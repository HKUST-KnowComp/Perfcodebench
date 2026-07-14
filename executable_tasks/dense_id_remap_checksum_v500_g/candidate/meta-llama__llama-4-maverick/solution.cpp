#include "interface.h"

#include <algorithm>
#include <unordered_map>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  std::unordered_map<uint32_t, uint32_t> remap_table;
  for (size_t i = 0; i < keys.size(); ++i) {
    remap_table[keys[i]] = values[i];
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t probe : probes) {
      auto it = remap_table.find(probe);
      if (it != remap_table.end()) {
        hash = mix(hash, it->second);
      }
    }
  }
  return hash;
}