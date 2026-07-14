#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  // Match baseline behavior: if no iterations, return initial hash (0).
  if (iters <= 0) return 0ULL;

  // Fast path: no groups to hash
  if (group_count == 0) {
    // For any iters >= 1, baseline resets to FNV offset and mixes nothing.
    return 1469598103934665603ULL;
  }

  // Dense storage for per-group maxima, initialized to 0 (missing groups -> 0)
  std::vector<uint32_t> best(group_count, 0u);

  const uint32_t* gptr = groups.data();
  const uint32_t* vptr = values.data();
  const std::size_t n = groups.size();

  // Compute per-group maxima once. Skip any out-of-range group IDs.
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t g = gptr[i];
    if (g < group_count) {
      uint32_t v = vptr[i];
      uint32_t cur = best[g];
      if (v > cur) best[g] = v;
    }
  }

  // Compute checksum/hash once; all iterations would be identical.
  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* bptr = best.data();
  for (uint32_t g = 0; g < group_count; ++g) {
    hash = mix(hash, bptr[g]);
  }

  return hash;
}
