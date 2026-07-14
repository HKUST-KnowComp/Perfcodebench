#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

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
  if (keys.empty() || probes.empty() || iters <= 0) {
    return 0;
  }

  // Find min and max keys to build direct lookup table
  uint32_t min_key = keys[0];
  uint32_t max_key = keys[0];
  for (size_t i = 1; i < keys.size(); ++i) {
    if (keys[i] < min_key) min_key = keys[i];
    if (keys[i] > max_key) max_key = keys[i];
  }

  // Build direct-access remap table
  const size_t table_size = static_cast<size_t>(max_key - min_key) + 1;
  std::vector<uint32_t> remap(table_size);
  for (size_t i = 0; i < keys.size(); ++i) {
    remap[keys[i] - min_key] = values[i];
  }

  const uint32_t* remap_ptr = remap.data();
  const uint32_t* probes_ptr = probes.data();
  const size_t n = probes.size();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    // Process in chunks of 4 for better ILP
    size_t i = 0;
    const size_t n4 = n & ~size_t(3);
    
    for (; i < n4; i += 4) {
      uint32_t p0 = probes_ptr[i];
      uint32_t p1 = probes_ptr[i + 1];
      uint32_t p2 = probes_ptr[i + 2];
      uint32_t p3 = probes_ptr[i + 3];
      
      uint32_t v0 = remap_ptr[p0 - min_key];
      uint32_t v1 = remap_ptr[p1 - min_key];
      uint32_t v2 = remap_ptr[p2 - min_key];
      uint32_t v3 = remap_ptr[p3 - min_key];
      
      hash = mix(hash, v0);
      hash = mix(hash, v1);
      hash = mix(hash, v2);
      hash = mix(hash, v3);
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      hash = mix(hash, remap_ptr[probes_ptr[i] - min_key]);
    }
  }

  return hash;
}