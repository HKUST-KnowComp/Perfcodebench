#include "interface.h"
#include <cstring>

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
  for (int iter = 0; iter < iters; ++iter) {
    memset(best.data(), 0, group_count * sizeof(uint32_t));
    const uint32_t* gptr = groups.data();
    const uint32_t* vptr = values.data();
    const std::size_t n = groups.size();
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t g = gptr[i];
      uint32_t v = vptr[i];
      if (v > best[g]) {
        best[g] = v;
      }
    }
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, best[g]);
    }
  }
  return hash;
}