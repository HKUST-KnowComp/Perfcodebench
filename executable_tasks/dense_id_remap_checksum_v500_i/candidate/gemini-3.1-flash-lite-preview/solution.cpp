#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (keys.empty()) return 0;

  uint32_t min_key = keys[0];
  uint32_t max_key = keys[0];
  for (uint32_t k : keys) {
    if (k < min_key) min_key = k;
    if (k > max_key) max_key = k;
  }

  std::vector<uint32_t> lookup(max_key - min_key + 1);
  for (size_t i = 0; i < keys.size(); ++i) {
    lookup[keys[i] - min_key] = values[i];
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = 1469598103934665603ULL;
    for (uint32_t probe : probes) {
      uint32_t val = lookup[probe - min_key];
      current_hash ^= static_cast<uint64_t>(val);
      current_hash *= 1099511628211ULL;
    }
    hash = current_hash;
  }
  return hash;
}