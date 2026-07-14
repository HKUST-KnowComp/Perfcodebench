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
    
    // Process in chunks to help with instruction-level parallelism
    std::size_t i = 0;
    
    // Main loop: compute sum directly as sum of (length * value)
    // No need to decode - just multiply run length by value
    for (; i + 3 < n; i += 4) {
      uint64_t len0 = run_ends[i] - prev;
      uint64_t len1 = run_ends[i + 1] - run_ends[i];
      uint64_t len2 = run_ends[i + 2] - run_ends[i + 1];
      uint64_t len3 = run_ends[i + 3] - run_ends[i + 2];
      
      sum += len0 * values[i];
      sum += len1 * values[i + 1];
      sum += len2 * values[i + 2];
      sum += len3 * values[i + 3];
      
      prev = run_ends[i + 3];
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      uint64_t len = run_ends[i] - prev;
      sum += len * values[i];
      prev = run_ends[i];
    }
  }
  
  return sum;
}