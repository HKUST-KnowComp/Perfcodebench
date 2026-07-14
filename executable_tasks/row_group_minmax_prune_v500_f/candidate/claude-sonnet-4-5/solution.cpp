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
      // Check if entire block can be pruned
      if (maxs[block_idx] < low || mins[block_idx] > high) {
        continue;
      }
      
      const size_t block_start = block_idx * block_size;
      const size_t block_end = (block_start + block_size < n) ? (block_start + block_size) : n;
      
      // Check if entire block is within range
      if (mins[block_idx] >= low && maxs[block_idx] <= high) {
        // All values in block are in range, sum directly
        uint64_t block_sum = 0;
        size_t i = block_start;
        
        // Unroll by 4 for better ILP
        for (; i + 3 < block_end; i += 4) {
          block_sum += static_cast<uint64_t>(values[i]);
          block_sum += static_cast<uint64_t>(values[i + 1]);
          block_sum += static_cast<uint64_t>(values[i + 2]);
          block_sum += static_cast<uint64_t>(values[i + 3]);
        }
        
        // Handle remainder
        for (; i < block_end; ++i) {
          block_sum += static_cast<uint64_t>(values[i]);
        }
        
        sum += block_sum;
      } else {
        // Block partially overlaps, need to check each value
        uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
        size_t i = block_start;
        
        // Unroll by 4 with independent accumulators
        for (; i + 3 < block_end; i += 4) {
          uint32_t v0 = values[i];
          uint32_t v1 = values[i + 1];
          uint32_t v2 = values[i + 2];
          uint32_t v3 = values[i + 3];
          
          if (v0 >= low && v0 <= high) s0 += v0;
          if (v1 >= low && v1 <= high) s1 += v1;
          if (v2 >= low && v2 <= high) s2 += v2;
          if (v3 >= low && v3 <= high) s3 += v3;
        }
        
        sum += s0 + s1 + s2 + s3;
        
        // Handle remainder
        for (; i < block_end; ++i) {
          uint32_t v = values[i];
          if (v >= low && v <= high) {
            sum += static_cast<uint64_t>(v);
          }
        }
      }
    }
  }
  
  return sum;
}