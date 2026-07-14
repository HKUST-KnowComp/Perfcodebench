#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (keys.empty()) return 0;

  uint32_t max_key = 0;
  for (uint32_t k : keys) {
    if (k > max_key) max_key = k;
  }

  std::vector<uint32_t> lookup(max_key + 1, 0);
  for (size_t i = 0; i < keys.size(); ++i) {
    lookup[keys[i]] = values[i];
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = 1469598103934665603ULL;
    for (uint32_t probe : probes) {
      uint32_t val = lookup[probe];
      current_hash ^= val;
      current_hash *= 1099511628211ULL;
    }
    hash = current_hash;
  }
  return hash;
}