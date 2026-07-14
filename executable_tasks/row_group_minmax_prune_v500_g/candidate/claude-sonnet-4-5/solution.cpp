#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  uint64_t sum = 0;
  const size_t n = values.size();
  const size_t num_blocks = (n + block_size - 1) / block_size;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    for (size_t block_idx = 0; block_idx < num_blocks; ++block_idx) {
      const uint32_t block_min = mins[block_idx];
      const uint32_t block_max = maxs[block_idx];
      
      // Prune: skip block if entirely outside range
      if (block_max < low || block_min > high) {
        continue;
      }
      
      const size_t block_start = block_idx * block_size;
      const size_t block_end = (block_start + block_size < n) ? block_start + block_size : n;
      
      // Fast path: entire block is within range
      if (block_min >= low && block_max <= high) {
        for (size_t i = block_start; i < block_end; ++i) {
          sum += static_cast<uint64_t>(values[i]);
        }
      } else {
        // Partial overlap: scan with range check
        uint64_t block_sum = 0;
        size_t i = block_start;
        
        // Unroll by 4 for better ILP
        for (; i + 3 < block_end; i += 4) {
          uint32_t v0 = values[i];
          uint32_t v1 = values[i + 1];
          uint32_t v2 = values[i + 2];
          uint32_t v3 = values[i + 3];
          
          if (v0 >= low && v0 <= high) block_sum += v0;
          if (v1 >= low && v1 <= high) block_sum += v1;
          if (v2 >= low && v2 <= high) block_sum += v2;
          if (v3 >= low && v3 <= high) block_sum += v3;
        }
        
        // Handle remainder
        for (; i < block_end; ++i) {
          uint32_t v = values[i];
          if (v >= low && v <= high) {
            block_sum += v;
          }
        }
        
        sum += block_sum;
      }
    }
  }
  
  return sum;
}