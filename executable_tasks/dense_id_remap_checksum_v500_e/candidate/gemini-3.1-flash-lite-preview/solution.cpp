#include "interface.h"
#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  if (keys.empty()) return 0;

  // Precompute a direct mapping table for O(1) lookup.
  // Assuming keys are within a reasonable range for dense mapping.
  uint32_t max_key = 0;
  for (uint32_t k : keys) if (k > max_key) max_key = k;

  std::vector<uint32_t> lookup(max_key + 1, 0);
  for (size_t i = 0; i < keys.size(); ++i) {
    lookup[keys[i]] = values[i];
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t probe : probes) {
      // Direct lookup is significantly faster than lower_bound
      hash = mix(hash, lookup[probe]);
    }
  }
  return hash;
}