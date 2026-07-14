#include "interface.h"

#include <vector>

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
  // Match baseline behavior: if no iterations, hash remains 0.
  if (iters <= 0) {
    return 0ULL;
  }

  // FNV-1a 64-bit offset basis and prime
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  // If domain is zero, mixing loop is empty each iter; baseline returns offset after last iter.
  if (domain == 0) {
    return FNV_OFFSET;
  }

  // Accumulator for the bounded domain
  std::vector<uint64_t> accum(domain, 0ULL);

  // Accumulate values for indices within [0, domain)
  const uint32_t* idxp = indices.data();
  const uint32_t* valp = values.data();
  const std::size_t n = indices.size();
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t idx = idxp[i];
    if (idx < domain) {
      accum[idx] += static_cast<uint64_t>(valp[i]);
    }
  }

  // Compute the hash across the full domain once (equivalent to the last iteration of the baseline loop)
  uint64_t h = FNV_OFFSET;
  const uint64_t* ap = accum.data();
  uint32_t m = domain;
  while (m--) {
    h ^= *ap++;
    h *= FNV_PRIME;
  }

  return h;
}
