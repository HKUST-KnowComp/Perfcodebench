#include "interface.h"

#include <algorithm>
#include <cstring>

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
  
  // Build direct lookup table
  if (keys.empty()) return 0;
  
  uint32_t max_key = keys.back();
  std::vector<uint32_t> remap_table(max_key + 1);
  
  for (size_t i = 0; i < keys.size(); ++i) {
    remap_table[keys[i]] = values[i];
  }
  
  uint64_t hash = 0;
  const size_t probe_count = probes.size();
  const uint32_t* probe_ptr = probes.data();
  const uint32_t* remap_ptr = remap_table.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    // Unroll by 4 for better ILP
    size_t i = 0;
    for (; i + 3 < probe_count; i += 4) {
      uint32_t p0 = probe_ptr[i];
      uint32_t p1 = probe_ptr[i + 1];
      uint32_t p2 = probe_ptr[i + 2];
      uint32_t p3 = probe_ptr[i + 3];
      
      hash = mix(hash, remap_ptr[p0]);
      hash = mix(hash, remap_ptr[p1]);
      hash = mix(hash, remap_ptr[p2]);
      hash = mix(hash, remap_ptr[p3]);
    }
    
    // Handle remainder
    for (; i < probe_count; ++i) {
      hash = mix(hash, remap_ptr[probe_ptr[i]]);
    }
  }
  
  return hash;
}