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
    
    // Process 4 runs at a time for better instruction-level parallelism
    std::size_t i = 0;
    const std::size_t unroll_limit = (n >= 4) ? (n - 3) : 0;
    
    for (; i < unroll_limit; i += 4) {
      const uint32_t end0 = run_ends[i];
      const uint32_t end1 = run_ends[i + 1];
      const uint32_t end2 = run_ends[i + 2];
      const uint32_t end3 = run_ends[i + 3];
      
      const uint64_t len0 = end0 - prev;
      const uint64_t len1 = end1 - end0;
      const uint64_t len2 = end2 - end1;
      const uint64_t len3 = end3 - end2;
      
      sum += len0 * values[i];
      sum += len1 * values[i + 1];
      sum += len2 * values[i + 2];
      sum += len3 * values[i + 3];
      
      prev = end3;
    }
    
    // Handle remaining runs
    for (; i < n; ++i) {
      const uint32_t end = run_ends[i];
      const uint64_t len = end - prev;
      sum += len * static_cast<uint64_t>(values[i]);
      prev = end;
    }
  }
  
  return sum;
}