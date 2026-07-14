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
      
      // Prune: skip blocks entirely outside range
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
        for (size_t i = block_start; i < block_end; ++i) {
          const uint32_t value = values[i];
          if (value >= low && value <= high) {
            sum += static_cast<uint64_t>(value);
          }
        }
      }
    }
  }
  
  return sum;
}