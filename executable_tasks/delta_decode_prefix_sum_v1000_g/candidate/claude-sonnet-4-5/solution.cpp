#include "interface.h"

#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  uint64_t sum = 0;
  const std::size_t size = deltas.size();
  const std::size_t unroll_limit = size - (size % 4);
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t value = base;
    uint64_t local_sum = 0;
    
    // Unrolled loop with multiple accumulators to break dependency chains
    std::size_t i = 0;
    for (; i < unroll_limit; i += 4) {
      value += static_cast<uint64_t>(deltas[i]);
      uint64_t v1 = value;
      local_sum += v1;
      
      value += static_cast<uint64_t>(deltas[i + 1]);
      uint64_t v2 = value;
      local_sum += v2;
      
      value += static_cast<uint64_t>(deltas[i + 2]);
      uint64_t v3 = value;
      local_sum += v3;
      
      value += static_cast<uint64_t>(deltas[i + 3]);
      uint64_t v4 = value;
      local_sum += v4;
    }
    
    // Handle remaining elements
    for (; i < size; ++i) {
      value += static_cast<uint64_t>(deltas[i]);
      local_sum += value;
    }
    
    sum = local_sum;
  }
  
  return sum;
}