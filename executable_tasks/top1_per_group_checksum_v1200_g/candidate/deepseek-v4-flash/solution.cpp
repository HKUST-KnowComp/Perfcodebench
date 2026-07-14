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
  const uint32_t* gdata = groups.data();
  const uint32_t* vdata = values.data();
  std::size_t n = groups.size();

  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t g = gdata[i];
      uint32_t v = vdata[i];
      if (v > best[g]) {
        best[g] = v;
      }
    }

    uint64_t h = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      h ^= best[g];
      h *= 1099511628211ULL;
      best[g] = 0;
    }
    hash = h;
  }
  return hash;
}
