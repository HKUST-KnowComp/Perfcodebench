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
    
    // Process in chunks of 4 for better ILP
    std::size_t i = 0;
    for (; i + 3 < probe_size; i += 4) {
      uint64_t s0 = static_cast<uint64_t>(lookup[probe_keys[i]]) * static_cast<uint64_t>(probe_weights[i]);
      uint64_t s1 = static_cast<uint64_t>(lookup[probe_keys[i+1]]) * static_cast<uint64_t>(probe_weights[i+1]);
      uint64_t s2 = static_cast<uint64_t>(lookup[probe_keys[i+2]]) * static_cast<uint64_t>(probe_weights[i+2]);
      uint64_t s3 = static_cast<uint64_t>(lookup[probe_keys[i+3]]) * static_cast<uint64_t>(probe_weights[i+3]);
      sum += s0 + s1 + s2 + s3;
    }
    
    // Handle remaining elements
    for (; i < probe_size; ++i) {
      sum += static_cast<uint64_t>(lookup[probe_keys[i]]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  
  return sum;
}