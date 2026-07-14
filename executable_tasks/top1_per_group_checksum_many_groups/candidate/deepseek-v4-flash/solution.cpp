#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  std::vector<uint32_t> best(group_count, 0);
  uint64_t hash = 0;
  const size_t n = groups.size();
  const uint32_t* g_ptr = groups.data();
  const uint32_t* v_ptr = values.data();
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      uint32_t g = g_ptr[i];
      uint32_t v = v_ptr[i];
      uint32_t& slot = best[g];
      if (v > slot) slot = v;
    }
    hash = 1469598103934665603ULL;
    uint32_t* b_ptr = best.data();
    for (uint32_t g = 0; g < group_count; ++g) {
      uint32_t val = b_ptr[g];
      hash ^= val;
      hash *= 1099511628211ULL;
      b_ptr[g] = 0;
    }
  }
  return hash;
}