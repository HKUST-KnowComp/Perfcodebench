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
      
      const size_t start = block_idx * block_size;
      const size_t end = (start + block_size < n) ? start + block_size : n;
      
      // Check if entire block is within range
      if (mins[block_idx] >= low && maxs[block_idx] <= high) {
        // All values in block are in range, sum without checks
        for (size_t i = start; i < end; ++i) {
          sum += static_cast<uint64_t>(values[i]);
        }
      } else {
        // Partial overlap, need to check each value
        for (size_t i = start; i < end; ++i) {
          uint32_t value = values[i];
          if (value >= low && value <= high) {
            sum += static_cast<uint64_t>(value);
          }
        }
      }
    }
  }
  
  return sum;
}