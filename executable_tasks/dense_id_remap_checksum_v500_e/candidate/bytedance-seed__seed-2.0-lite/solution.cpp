#include "interface.h"

#include <vector>

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
  std::vector<uint32_t> lookup;
  if (!keys.empty()) {
    const uint32_t max_key = keys.back();
    lookup.resize(max_key + 1);
    for (size_t i = 0; i < keys.size(); ++i) {
      lookup[keys[i]] = values[i];
    }
  }

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t probe : probes) {
      hash = mix(hash, lookup[probe]);
    }
    final_hash = hash;
  }
  return final_hash;
}