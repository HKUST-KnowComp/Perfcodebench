#include "interface.h"

#include <cstdint>
#include <limits>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  
  if (dim_keys.empty() || probe_keys.empty()) {
    return 0;
  }

  // For a dense dimension table, keys are contiguous starting from min_key
  // So we can use direct indexing: index = key - min_key
  const uint32_t min_key = dim_keys[0];
  const uint32_t* dim_vals = dim_values.data();
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_weights = probe_weights.data();
  const std::size_t n = probe_keys.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    
    std::size_t i = 0;
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);
    
    // Unrolled loop - 4 elements at a time
    for (; i < n4; i += 4) {
      const uint32_t k0 = p_keys[i];
      const uint32_t k1 = p_keys[i + 1];
      const uint32_t k2 = p_keys[i + 2];
      const uint32_t k3 = p_keys[i + 3];
      
      const uint32_t w0 = p_weights[i];
      const uint32_t w1 = p_weights[i + 1];
      const uint32_t w2 = p_weights[i + 2];
      const uint32_t w3 = p_weights[i + 3];
      
      const uint32_t v0 = dim_vals[k0 - min_key];
      const uint32_t v1 = dim_vals[k1 - min_key];
      const uint32_t v2 = dim_vals[k2 - min_key];
      const uint32_t v3 = dim_vals[k3 - min_key];
      
      s0 += static_cast<uint64_t>(v0) * static_cast<uint64_t>(w0);
      s1 += static_cast<uint64_t>(v1) * static_cast<uint64_t>(w1);
      s2 += static_cast<uint64_t>(v2) * static_cast<uint64_t>(w2);
      s3 += static_cast<uint64_t>(v3) * static_cast<uint64_t>(w3);
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      const uint32_t k = p_keys[i];
      const uint32_t w = p_weights[i];
      const uint32_t v = dim_vals[k - min_key];
      s0 += static_cast<uint64_t>(v) * static_cast<uint64_t>(w);
    }
    
    sum = s0 + s1 + s2 + s3;
  }
  
  return sum;
}