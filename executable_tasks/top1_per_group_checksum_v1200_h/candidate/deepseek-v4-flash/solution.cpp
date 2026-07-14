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
  std::vector<uint32_t> best(group_count, 0);
  std::vector<uint64_t> version(group_count, 0);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t cur_version = iter + 1;
    const std::size_t n = groups.size();
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t g = groups[i];
      if (version[g] != cur_version) {
        version[g] = cur_version;
        best[g] = values[i];
      } else {
        uint32_t v = values[i];
        if (v > best[g]) best[g] = v;
      }
    }
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, version[g] == cur_version ? best[g] : 0u);
    }
  }
  return hash;
}
