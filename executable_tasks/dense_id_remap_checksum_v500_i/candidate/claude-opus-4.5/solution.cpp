#include "interface.h"

#include <cstdint>
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
  
  if (keys.empty() || probes.empty() || iters <= 0) {
    return 1469598103934665603ULL;
  }

  // Find min and max keys to build a direct lookup table
  uint32_t min_key = keys.front();
  uint32_t max_key = keys.back();
  
  // Build direct lookup table: table[key - min_key] = value
  std::vector<uint32_t> table(static_cast<size_t>(max_key - min_key + 1));
  
  const size_t n_keys = keys.size();
  for (size_t i = 0; i < n_keys; ++i) {
    table[keys[i] - min_key] = values[i];
  }
  
  const uint32_t* table_ptr = table.data();
  const uint32_t* probes_ptr = probes.data();
  const size_t n_probes = probes.size();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    size_t i = 0;
    const size_t n4 = n_probes & ~size_t(3);
    
    // Unrolled loop - process 4 probes at a time
    for (; i < n4; i += 4) {
      uint32_t p0 = probes_ptr[i];
      uint32_t p1 = probes_ptr[i + 1];
      uint32_t p2 = probes_ptr[i + 2];
      uint32_t p3 = probes_ptr[i + 3];
      
      uint32_t v0 = table_ptr[p0 - min_key];
      uint32_t v1 = table_ptr[p1 - min_key];
      uint32_t v2 = table_ptr[p2 - min_key];
      uint32_t v3 = table_ptr[p3 - min_key];
      
      hash = mix(hash, v0);
      hash = mix(hash, v1);
      hash = mix(hash, v2);
      hash = mix(hash, v3);
    }
    
    // Handle remaining probes
    for (; i < n_probes; ++i) {
      uint32_t p = probes_ptr[i];
      hash = mix(hash, table_ptr[p - min_key]);
    }
  }
  
  return hash;
}