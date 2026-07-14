#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

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
  std::vector<uint32_t> best(group_count);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(best.begin(), best.end(), 0);
    const std::size_t n = groups.size();
    const uint32_t* gptr = groups.data();
    const uint32_t* vptr = values.data();
    uint32_t* bptr = best.data();
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t g = gptr[i];
      uint32_t v = vptr[i];
      if (v > bptr[g]) {
        bptr[g] = v;
      }
    }
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, bptr[g]);
    }
  }
  return hash;
}