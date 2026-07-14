#include "interface.h"

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  const uint32_t* dim_vals = dim_values.data();
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_weights = probe_weights.data();
  const std::size_t probe_size = probe_keys.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;
    
    // Unroll by 4 for better ILP
    for (; i + 3 < probe_size; i += 4) {
      uint64_t s0 = static_cast<uint64_t>(dim_vals[p_keys[i]]) * static_cast<uint64_t>(p_weights[i]);
      uint64_t s1 = static_cast<uint64_t>(dim_vals[p_keys[i+1]]) * static_cast<uint64_t>(p_weights[i+1]);
      uint64_t s2 = static_cast<uint64_t>(dim_vals[p_keys[i+2]]) * static_cast<uint64_t>(p_weights[i+2]);
      uint64_t s3 = static_cast<uint64_t>(dim_vals[p_keys[i+3]]) * static_cast<uint64_t>(p_weights[i+3]);
      sum += s0 + s1 + s2 + s3;
    }
    
    // Handle remainder
    for (; i < probe_size; ++i) {
      sum += static_cast<uint64_t>(dim_vals[p_keys[i]]) * static_cast<uint64_t>(p_weights[i]);
    }
  }
  return sum;
}