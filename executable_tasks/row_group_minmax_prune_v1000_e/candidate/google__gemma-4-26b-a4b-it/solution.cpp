#include "interface.h"
#include <vector>
#include <numeric>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n = values.size();
  const size_t num_blocks = (n + block_size - 1) / block_size;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t b = 0; b < num_blocks; ++b) {
      uint32_t b_min = mins[b];
      uint32_t b_max = maxs[b];

      // Case 1: Block is entirely outside the range
      if (b_max < low || b_min > high) {
        continue;
      }

      // Case 2: Block is entirely within the range
      if (b_min >= low && b_max <= high) {
        size_t start = b * block_size;
        size_t end = (start + block_size > n) ? n : start + block_size;
        for (size_t i = start; i < end; ++i) {
          current_iter_sum += values[i];
        }
      } 
      // Case 3: Block partially overlaps the range
      else {
        size_t start = b * block_size;
        size_t end = (start + block_size > n) ? n : start + block_size;
        for (size_t i = start; i < end; ++i) {
          uint32_t val = values[i];
          if (val >= low && val <= high) {
            current_iter_sum += val;
          }
        }
      }
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}