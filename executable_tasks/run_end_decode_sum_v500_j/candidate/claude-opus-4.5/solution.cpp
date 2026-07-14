#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t sum = 0;
  const std::size_t n = values.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    uint32_t prev = 0;
    
    // Process in chunks for better instruction-level parallelism
    std::size_t i = 0;
    
    // Main loop with manual unrolling for ILP
    for (; i + 4 <= n; i += 4) {
      uint32_t end0 = run_ends[i];
      uint32_t end1 = run_ends[i + 1];
      uint32_t end2 = run_ends[i + 2];
      uint32_t end3 = run_ends[i + 3];
      
      uint64_t len0 = end0 - prev;
      uint64_t len1 = end1 - end0;
      uint64_t len2 = end2 - end1;
      uint64_t len3 = end3 - end2;
      
      sum += len0 * values[i];
      sum += len1 * values[i + 1];
      sum += len2 * values[i + 2];
      sum += len3 * values[i + 3];
      
      prev = end3;
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      uint32_t end = run_ends[i];
      uint64_t len = end - prev;
      sum += len * values[i];
      prev = end;
    }
  }
  
  return sum;
}