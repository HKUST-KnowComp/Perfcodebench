#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
  // Preserve baseline semantics for iters == 0
  if (iters <= 0) return 0ULL;

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  // If domain is zero, hashing loop is empty; baseline returns FNV offset per iteration
  if (domain == 0) return FNV_OFFSET;

  // Dense accumulator over bounded domain
  std::vector<uint64_t> accum(static_cast<std::size_t>(domain), 0ULL);

  // Accumulate only in-range indices; out-of-range indices do not affect final hash
  const std::size_t n = indices.size();
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t idx = indices[i];
    if (idx < domain) {
      accum[idx] += static_cast<uint64_t>(values[i]);
    }
  }

  // Compute FNV-1a-like hash across the entire bounded domain in order
  uint64_t hash = FNV_OFFSET;
  const uint64_t* data = accum.data();
  const std::size_t D = static_cast<std::size_t>(domain);
  for (std::size_t k = 0; k < D; ++k) {
    hash ^= data[k];
    hash *= FNV_PRIME;
  }

  return hash;
}
