#include "interface.h"

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
  
  // Precompute indices once instead of binary searching every iteration
  std::vector<std::size_t> indices;
  indices.reserve(probes.size());
  
  for (uint32_t probe : probes) {
    const auto it = std::lower_bound(keys.begin(), keys.end(), probe);
    indices.push_back(static_cast<std::size_t>(it - keys.begin()));
  }
  
  uint64_t hash = 0;
  const std::size_t probe_count = indices.size();
  const uint32_t* values_data = values.data();
  const std::size_t* indices_data = indices.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    // Unroll by 4 for better ILP
    std::size_t i = 0;
    const std::size_t unroll_limit = probe_count - (probe_count % 4);
    
    for (; i < unroll_limit; i += 4) {
      uint64_t v0 = values_data[indices_data[i]];
      uint64_t v1 = values_data[indices_data[i + 1]];
      uint64_t v2 = values_data[indices_data[i + 2]];
      uint64_t v3 = values_data[indices_data[i + 3]];
      
      hash = mix(hash, v0);
      hash = mix(hash, v1);
      hash = mix(hash, v2);
      hash = mix(hash, v3);
    }
    
    // Handle remainder
    for (; i < probe_count; ++i) {
      hash = mix(hash, values_data[indices_data[i]]);
    }
  }
  
  return hash;
}