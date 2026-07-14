#include "interface.h"
#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint32_t value) {
  hash ^= static_cast<uint64_t>(value);
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (keys.empty()) return 0;

  // Precompute a lookup table for O(1) access.
  // Since keys are sorted and dense, we map probe -> index.
  uint32_t min_key = keys.front();
  uint32_t max_key = keys.back();
  uint32_t range = max_key - min_key + 1;

  std::vector<uint32_t> lookup(range, 0);
  for (size_t i = 0; i < keys.size(); ++i) {
    lookup[keys[i] - min_key] = values[i];
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = 1469598103934665603ULL;
    for (uint32_t probe : probes) {
      // Assuming probes are within the range of keys as per standard dense id tasks
      uint32_t val = lookup[probe - min_key];
      current_hash = mix(current_hash, val);
    }
    hash = current_hash;
  }
  return hash;
}