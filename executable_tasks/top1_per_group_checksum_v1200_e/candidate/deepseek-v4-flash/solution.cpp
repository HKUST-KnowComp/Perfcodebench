#include "interface.h"

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  std::vector<uint32_t> best(group_count, 0);
  const size_t n = groups.size();
  const uint32_t* gptr = groups.data();
  const uint32_t* vptr = values.data();
  uint32_t* bptr = best.data();
  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    for (uint32_t g = 0; g < group_count; ++g) bptr[g] = 0;
    for (size_t i = 0; i < n; ++i) {
      const uint32_t g = gptr[i];
      const uint32_t v = vptr[i];
      if (v > bptr[g]) bptr[g] = v;
    }
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash ^= bptr[g];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
