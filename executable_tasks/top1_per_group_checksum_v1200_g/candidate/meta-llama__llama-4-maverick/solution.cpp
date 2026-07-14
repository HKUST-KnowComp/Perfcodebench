#include "interface.h"

#include <unordered_map>

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
  std::unordered_map<uint32_t, uint32_t> best;
  best.reserve(group_count);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    best.clear();
    for (std::size_t i = 0; i < groups.size(); ++i) {
      uint32_t group = groups[i];
      uint32_t value = values[i];
      auto it = best.find(group);
      if (it == best.end()) {
        best.emplace(group, value);
      } else if (value > it->second) {
        it->second = value;
      }
    }
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      const auto it = best.find(g);
      hash = mix(hash, it == best.end() ? 0u : it->second);
    }
  }
  return hash;
}
