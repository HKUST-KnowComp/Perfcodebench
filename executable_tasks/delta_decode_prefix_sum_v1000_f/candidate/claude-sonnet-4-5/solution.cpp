#include "interface.h"

#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  uint64_t sum = 0;
  const std::size_t n = deltas.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t value = base;
    sum = 0;
    
    // Process 4 elements at a time with separate accumulators
    std::size_t i = 0;
    const std::size_t limit = n - (n % 4);
    
    uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    
    for (; i < limit; i += 4) {
      value += deltas[i];
      sum0 += value;
      
      value += deltas[i + 1];
      sum1 += value;
      
      value += deltas[i + 2];
      sum2 += value;
      
      value += deltas[i + 3];
      sum3 += value;
    }
    
    sum = sum0 + sum1 + sum2 + sum3;
    
    // Handle remaining elements
    for (; i < n; ++i) {
      value += deltas[i];
      sum += value;
    }
  }
  
  return sum;
}