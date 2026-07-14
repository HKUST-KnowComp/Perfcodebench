#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
  std::vector<uint32_t> best(group_count, 0);
  uint64_t hash = 0;
  const size_t n = groups.size();

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(best.begin(), best.end(), 0u);
    for (size_t i = 0; i < n; ++i) {
      uint32_t g = groups[i];
      uint32_t v = values[i];
      if (v > best[g]) best[g] = v;
    }
    uint64_t h = 1469598103934665603ULL;
    uint32_t* b = best.data();
    size_t g = 0;
    // 4-way unroll
    for (; g + 3 < group_count; g += 4) {
      h = mix(h, b[g]);
      h = mix(h, b[g+1]);
      h = mix(h, b[g+2]);
      h = mix(h, b[g+3]);
    }
    for (; g < group_count; ++g) {
      h = mix(h, b[g]);
    }
    hash = h;
  }
  return hash;
}