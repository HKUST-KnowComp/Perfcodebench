#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  const size_t n = groups.size();
  std::vector<uint32_t> best(group_count, 0);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Update per-group maximums
    const uint32_t* gptr = groups.data();
    const uint32_t* vptr = values.data();
    for (size_t i = 0; i < n; ++i) {
      uint32_t g = gptr[i];
      uint32_t v = vptr[i];
      if (v > best[g]) best[g] = v;
    }

    // Compute hash and reset for next iteration in one pass
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      uint32_t val = best[g];
      hash ^= val;
      hash *= 1099511628211ULL;
      best[g] = 0;
    }
    final_hash = hash;
  }
  return final_hash;
}
