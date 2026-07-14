#include "interface.h"

#include <cstdint>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  const std::size_t n = probe_keys.size();
  const uint32_t* __restrict__ keys = probe_keys.data();
  const uint32_t* __restrict__ weights = probe_weights.data();
  const uint32_t* __restrict__ values = dim_values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    
    std::size_t i = 0;
    const std::size_t n_unroll = (n / 4) * 4;
    
    for (; i < n_unroll; i += 4) {
      __builtin_prefetch(&values[keys[i + 8]], 0, 0);
      __builtin_prefetch(&values[keys[i + 9]], 0, 0);
      
      uint32_t k0 = keys[i];
      uint32_t k1 = keys[i + 1];
      uint32_t k2 = keys[i + 2];
      uint32_t k3 = keys[i + 3];
      
      uint32_t v0 = values[k0];
      uint32_t v1 = values[k1];
      uint32_t v2 = values[k2];
      uint32_t v3 = values[k3];
      
      sum0 += static_cast<uint64_t>(v0) * static_cast<uint64_t>(weights[i]);
      sum1 += static_cast<uint64_t>(v1) * static_cast<uint64_t>(weights[i + 1]);
      sum2 += static_cast<uint64_t>(v2) * static_cast<uint64_t>(weights[i + 2]);
      sum3 += static_cast<uint64_t>(v3) * static_cast<uint64_t>(weights[i + 3]);
    }
    
    for (; i < n; ++i) {
      sum0 += static_cast<uint64_t>(values[keys[i]]) * static_cast<uint64_t>(weights[i]);
    }
    
    sum = sum0 + sum1 + sum2 + sum3;
  }
  return sum;
}