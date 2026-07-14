#include "interface.h"

#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  uint64_t sum = 0;
  const std::size_t size = deltas.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t value = base;
    uint64_t local_sum = 0;
    
    // Unroll by 4 with separate accumulators
    std::size_t i = 0;
    const std::size_t unroll_limit = size - (size % 4);
    
    uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    uint64_t v0 = value, v1, v2, v3;
    
    for (; i < unroll_limit; i += 4) {
      v0 += deltas[i];
      sum0 += v0;
      
      v1 = v0 + deltas[i + 1];
      sum1 += v1;
      
      v2 = v1 + deltas[i + 2];
      sum2 += v2;
      
      v3 = v2 + deltas[i + 3];
      sum3 += v3;
      
      v0 = v3;
    }
    
    value = v0;
    local_sum = sum0 + sum1 + sum2 + sum3;
    
    // Handle remainder
    for (; i < size; ++i) {
      value += deltas[i];
      local_sum += value;
    }
    
    sum = local_sum;
  }
  
  return sum;
}