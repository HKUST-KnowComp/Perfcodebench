#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  const uint32_t* gptr = groups.data();
  const uint32_t* vptr = values.data();
  const size_t n = groups.size();
  std::vector<uint32_t> best(group_count);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // reset maxima to 0
    for (uint32_t g = 0; g < group_count; ++g) {
      best[g] = 0;
    }
    // compute per-group max
    for (size_t i = 0; i < n; ++i) {
      uint32_t g = gptr[i];
      uint32_t v = vptr[i];
      if (v > best[g]) best[g] = v;
    }
    // compute hash over all groups
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash ^= best[g];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}