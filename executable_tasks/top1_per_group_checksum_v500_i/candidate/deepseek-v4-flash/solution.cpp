#include "interface.h"

#include <cstdint>
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
  const std::size_t n = groups.size();
  const uint32_t gc = group_count;
  std::vector<uint32_t> max_vals(gc, 0);

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t g = groups[i];
    uint32_t v = values[i];
    if (v > max_vals[g]) {
      max_vals[g] = v;
    }
  }

  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t g = 0; g < gc; ++g) {
    hash = mix(hash, max_vals[g]);
  }
  return hash;
}
