#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  
  uint64_t final_sum = 0;
  const size_t num_values = values.size();
  const size_t num_blocks = mins.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    
    for (size_t b = 0; b < num_blocks; ++b) {
      const uint32_t b_min = mins[b];
      const uint32_t b_max = maxs[b];

      // Case 1: Entire block is outside the range [low, high]
      if (b_min > high || b_max < low) {
        continue;
      }

      const size_t start = b * block_size;
      const size_t end = std::min(start + block_size, num_values);

      // Case 2: Entire block is inside the range [low, high]
      if (b_min >= low && b_max <= high) {
        uint64_t block_sum = 0;
        #pragma omp simd reduction(+:block_sum)
        for (size_t i = start; i < end; ++i) {
          block_sum += values[i];
        }
        current_iter_sum += block_sum;
      } 
      // Case 3: Block partially overlaps with the range [low, high]
      else {
        uint64_t block_sum = 0;
        #pragma omp simd reduction(+:block_sum)
        for (size_t i = start; i < end; ++i) {
          const uint32_t v = values[i];
          if (v >= low && v <= high) {
            block_sum += v;
          }
        }
        current_iter_sum += block_sum;
      }
    }
    final_sum = current_iter_sum;
  }

  return final_sum;
}