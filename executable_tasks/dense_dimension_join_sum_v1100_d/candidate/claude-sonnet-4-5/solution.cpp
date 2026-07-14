#include "interface.h"

#include <algorithm>
#include <cstring>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  
  // Build direct lookup array for dense dimension table
  uint32_t max_key = 0;
  for (const auto& key : dim_keys) {
    if (key > max_key) max_key = key;
  }
  
  std::vector<uint32_t> lookup(max_key + 1, 0);
  for (std::size_t i = 0; i < dim_keys.size(); ++i) {
    lookup[dim_keys[i]] = dim_values[i];
  }
  
  uint64_t sum = 0;
  const std::size_t probe_size = probe_keys.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    // Process in chunks of 4 for loop unrolling
    std::size_t i = 0;
    const std::size_t unroll_limit = probe_size - (probe_size % 4);
    
    for (; i < unroll_limit; i += 4) {
      // Prefetch next iteration data
      if (i + 8 < probe_size) {
        __builtin_prefetch(&lookup[probe_keys[i + 4]], 0, 1);
        __builtin_prefetch(&lookup[probe_keys[i + 5]], 0, 1);
      }
      
      sum += static_cast<uint64_t>(lookup[probe_keys[i]]) * static_cast<uint64_t>(probe_weights[i]);
      sum += static_cast<uint64_t>(lookup[probe_keys[i + 1]]) * static_cast<uint64_t>(probe_weights[i + 1]);
      sum += static_cast<uint64_t>(lookup[probe_keys[i + 2]]) * static_cast<uint64_t>(probe_weights[i + 2]);
      sum += static_cast<uint64_t>(lookup[probe_keys[i + 3]]) * static_cast<uint64_t>(probe_weights[i + 3]);
    }
    
    // Handle remaining elements
    for (; i < probe_size; ++i) {
      sum += static_cast<uint64_t>(lookup[probe_keys[i]]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  
  return sum;
}