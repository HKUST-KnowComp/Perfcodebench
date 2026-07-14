#include "interface.h"

#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    const std::size_t size = values.size();
    std::size_t i = 0;
    
    // Process 4 elements at a time for better ILP
    for (; i + 3 < size; i += 4) {
      sum += static_cast<uint64_t>(values[i]) * lengths[i];
      sum += static_cast<uint64_t>(values[i+1]) * lengths[i+1];
      sum += static_cast<uint64_t>(values[i+2]) * lengths[i+2];
      sum += static_cast<uint64_t>(values[i+3]) * lengths[i+3];
    }
    
    // Handle remaining elements
    for (; i < size; ++i) {
      sum += static_cast<uint64_t>(values[i]) * lengths[i];
    }
  }
  
  return sum;
}