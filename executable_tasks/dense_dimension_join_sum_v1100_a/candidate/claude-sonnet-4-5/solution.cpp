#include "interface.h"

#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  
  if (dim_keys.empty() || probe_keys.empty()) {
    return 0;
  }
  
  // For dense dimension tables, keys are consecutive
  // We can use direct indexing instead of binary search
  const uint32_t min_key = dim_keys[0];
  const uint32_t* dim_vals_ptr = dim_values.data();
  const uint32_t* probe_keys_ptr = probe_keys.data();
  const uint32_t* probe_weights_ptr = probe_weights.data();
  const std::size_t probe_size = probe_keys.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    // Process in chunks of 4 for loop unrolling
    std::size_t i = 0;
    const std::size_t unroll_limit = probe_size - (probe_size % 4);
    
    for (; i < unroll_limit; i += 4) {
      // Prefetch next iteration data
      if (i + 8 < probe_size) {
        __builtin_prefetch(&probe_keys_ptr[i + 8], 0, 0);
        __builtin_prefetch(&probe_weights_ptr[i + 8], 0, 0);
      }
      
      const uint32_t idx0 = probe_keys_ptr[i] - min_key;
      const uint32_t idx1 = probe_keys_ptr[i + 1] - min_key;
      const uint32_t idx2 = probe_keys_ptr[i + 2] - min_key;
      const uint32_t idx3 = probe_keys_ptr[i + 3] - min_key;
      
      sum += static_cast<uint64_t>(dim_vals_ptr[idx0]) * static_cast<uint64_t>(probe_weights_ptr[i]);
      sum += static_cast<uint64_t>(dim_vals_ptr[idx1]) * static_cast<uint64_t>(probe_weights_ptr[i + 1]);
      sum += static_cast<uint64_t>(dim_vals_ptr[idx2]) * static_cast<uint64_t>(probe_weights_ptr[i + 2]);
      sum += static_cast<uint64_t>(dim_vals_ptr[idx3]) * static_cast<uint64_t>(probe_weights_ptr[i + 3]);
    }
    
    // Handle remaining elements
    for (; i < probe_size; ++i) {
      const uint32_t idx = probe_keys_ptr[i] - min_key;
      sum += static_cast<uint64_t>(dim_vals_ptr[idx]) * static_cast<uint64_t>(probe_weights_ptr[i]);
    }
  }
  
  return sum;
}