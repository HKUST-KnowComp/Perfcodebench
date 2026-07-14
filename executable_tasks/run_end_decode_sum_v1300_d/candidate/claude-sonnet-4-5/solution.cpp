#include "interface.h"

#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t sum = 0;
  const std::size_t size = values.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    uint32_t prev = 0;
    
    // Process in chunks of 4 for better ILP
    std::size_t i = 0;
    for (; i + 3 < size; i += 4) {
      const uint32_t len0 = run_ends[i] - prev;
      const uint64_t contrib0 = static_cast<uint64_t>(values[i]) * len0;
      
      const uint32_t len1 = run_ends[i+1] - run_ends[i];
      const uint64_t contrib1 = static_cast<uint64_t>(values[i+1]) * len1;
      
      const uint32_t len2 = run_ends[i+2] - run_ends[i+1];
      const uint64_t contrib2 = static_cast<uint64_t>(values[i+2]) * len2;
      
      const uint32_t len3 = run_ends[i+3] - run_ends[i+2];
      const uint64_t contrib3 = static_cast<uint64_t>(values[i+3]) * len3;
      
      sum += contrib0 + contrib1 + contrib2 + contrib3;
      prev = run_ends[i+3];
    }
    
    // Handle remaining elements
    for (; i < size; ++i) {
      const uint32_t len = run_ends[i] - prev;
      sum += static_cast<uint64_t>(values[i]) * len;
      prev = run_ends[i];
    }
  }
  
  return sum;
}