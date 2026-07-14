#include "interface.h"

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  const std::size_t probe_size = probe_keys.size();
  const std::size_t unroll_limit = probe_size - (probe_size % 4);
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    
    // Unrolled loop for better ILP
    std::size_t i = 0;
    for (; i < unroll_limit; i += 4) {
      sum0 += static_cast<uint64_t>(dim_values[probe_keys[i]]) * static_cast<uint64_t>(probe_weights[i]);
      sum1 += static_cast<uint64_t>(dim_values[probe_keys[i+1]]) * static_cast<uint64_t>(probe_weights[i+1]);
      sum2 += static_cast<uint64_t>(dim_values[probe_keys[i+2]]) * static_cast<uint64_t>(probe_weights[i+2]);
      sum3 += static_cast<uint64_t>(dim_values[probe_keys[i+3]]) * static_cast<uint64_t>(probe_weights[i+3]);
    }
    
    // Handle remaining elements
    for (; i < probe_size; ++i) {
      sum0 += static_cast<uint64_t>(dim_values[probe_keys[i]]) * static_cast<uint64_t>(probe_weights[i]);
    }
    
    sum = sum0 + sum1 + sum2 + sum3;
  }
  return sum;
}