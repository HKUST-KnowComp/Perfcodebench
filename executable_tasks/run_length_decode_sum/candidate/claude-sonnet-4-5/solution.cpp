#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t size = values.size();
    std::size_t i = 0;
    
    // Process 4 elements at a time for better ILP
    for (; i + 3 < size; i += 4) {
      uint64_t s0 = static_cast<uint64_t>(values[i]) * lengths[i];
      uint64_t s1 = static_cast<uint64_t>(values[i+1]) * lengths[i+1];
      uint64_t s2 = static_cast<uint64_t>(values[i+2]) * lengths[i+2];
      uint64_t s3 = static_cast<uint64_t>(values[i+3]) * lengths[i+3];
      sum += s0 + s1 + s2 + s3;
    }
    
    // Handle remaining elements
    for (; i < size; ++i) {
      sum += static_cast<uint64_t>(values[i]) * lengths[i];
    }
  }
  
  return sum;
}