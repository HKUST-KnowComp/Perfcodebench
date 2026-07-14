#include "interface.h"

#include <vector>

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
    for (std::size_t i = 0; i < groups.size(); ++i) {
      uint32_t g = groups[i];
      if (values[i] > best[g]) {
        best[g] = values[i];
      }
    }
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, best[g]);
    }
  }
  return hash;
}