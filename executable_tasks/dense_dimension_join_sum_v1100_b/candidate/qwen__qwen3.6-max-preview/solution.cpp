#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  
  uint64_t sum = 0;
  if (dim_keys.empty() || probe_keys.empty()) return 0;
  
  // Dense dimension implies contiguous keys: dim_keys[i] == dim_keys[0] + i
  const uint32_t offset = dim_keys[0];
  const uint32_t* d_vals = dim_values.data();
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_weights = probe_weights.data();
  const size_t n = probe_keys.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    size_t i = 0;
    
    // Manual 4x unroll with independent accumulators for ILP
    for (; i + 3 < n; i += 4) {
      s0 += (uint64_t)d_vals[p_keys[i] - offset] * p_weights[i];
      s1 += (uint64_t)d_vals[p_keys[i+1] - offset] * p_weights[i+1];
      s2 += (uint64_t)d_vals[p_keys[i+2] - offset] * p_weights[i+2];
      s3 += (uint64_t)d_vals[p_keys[i+3] - offset] * p_weights[i+3];
    }
    
    uint64_t current_sum = s0 + s1 + s2 + s3;
    for (; i < n; ++i) {
      current_sum += (uint64_t)d_vals[p_keys[i] - offset] * p_weights[i];
    }
    sum = current_sum;
  }
  return sum;
}