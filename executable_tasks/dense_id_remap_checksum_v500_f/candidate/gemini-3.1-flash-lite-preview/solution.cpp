#include "interface.h"
#include <vector>
#include <algorithm>

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
} // namespace

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
  const uint32_t* lookup_ptr = lookup.data();
  const uint32_t offset = min_key;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = 1469598103934665603ULL;
    for (uint32_t probe : probes) {
      current_hash = mix(current_hash, lookup_ptr[probe - offset]);
    }
    hash = current_hash;
  }
  return hash;
}