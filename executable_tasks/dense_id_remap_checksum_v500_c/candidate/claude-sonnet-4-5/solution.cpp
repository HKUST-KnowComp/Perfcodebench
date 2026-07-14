#include "interface.h"

#include <algorithm>

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
  // Build direct lookup table for O(1) access
  if (keys.empty()) {
    return 1469598103934665603ULL;
  }
  
  uint32_t max_key = *std::max_element(keys.begin(), keys.end());
  std::vector<uint32_t> remap(max_key + 1);
  
  for (size_t i = 0; i < keys.size(); ++i) {
    remap[keys[i]] = values[i];
  }
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t probe : probes) {
      // Direct lookup instead of binary search
      if (probe <= max_key) {
        hash = mix(hash, remap[probe]);
      } else {
        // Handle out-of-bounds probe
        hash = mix(hash, values[keys.size()]);
      }
    }
  }
  return hash;
}
