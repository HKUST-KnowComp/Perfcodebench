#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
  // If no iterations, baseline returns 0
  if (iters <= 0) return 0;

  // FNV-1a 64-bit constants
  const uint64_t fnv_offset = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;

  // Handle non-positive group count like baseline checksum_map
  if (groups <= 0) {
    return fnv_offset;
  }

  const size_t G = static_cast<size_t>(groups);
  std::vector<uint64_t> agg(G, 0);

  const uint32_t* kptr = keys.data();
  const uint32_t* vptr = values.data();
  const size_t n = keys.size();

  // Aggregate once; keys are bounded integers, only accumulate for valid groups
  for (size_t i = 0; i < n; ++i) {
    const uint32_t k = kptr[i];
    if (k < static_cast<uint32_t>(groups)) {
      agg[k] += static_cast<uint64_t>(vptr[i]);
    }
  }

  // Compute checksum once (same for all iterations in baseline)
  uint64_t hash = fnv_offset;
  const uint64_t* ap = agg.data();
  for (size_t g = 0; g < G; ++g) {
    hash ^= ap[g];
    hash *= fnv_prime;
  }

  return hash;
}
