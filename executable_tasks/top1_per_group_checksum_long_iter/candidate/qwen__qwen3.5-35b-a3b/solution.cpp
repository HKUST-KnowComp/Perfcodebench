#include "interface.h"

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
  std::vector<uint32_t> max_vals(group_count);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(max_vals.begin(), max_vals.end(), 0);
    for (std::size_t i = 0; i < groups.size(); ++i) {
      uint32_t group = groups[i];
      uint32_t value = values[i];
      if (value > max_vals[group]) {
        max_vals[group] = value;
      }
    }
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, max_vals[g]);
    }
  }
  return hash;
}