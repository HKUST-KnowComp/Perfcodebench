#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  uint64_t total_sum = 0;
  size_t num_blocks = mins.size();
  size_t n = values.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < num_blocks; ++i) {
      uint32_t b_min = mins[i];
      uint32_t b_max = maxs[i];

      // Case 1: Block is entirely outside the range [low, high]
      if (b_max < low || b_min > high) {
        continue;
      }

      // Case 2: Block is entirely within the range [low, high]
      if (b_min >= low && b_max <= high) {
        size_t start = i * block_size;
        size_t end = std::min(start + block_size, n);
        for (size_t j = start; j < end; ++j) {
          current_iter_sum += static_cast<uint64_t>(values[j]);
        }
      }
      // Case 3: Block partially overlaps the range
      else {
        size_t start = i * block_size;
        size_t end = std::min(start + block_size, n);
        for (size_t j = start; j < end; ++j) {
          uint32_t val = values[j];
          if (val >= low && val <= high) {
            current_iter_sum += static_cast<uint64_t>(val);
          }
        }
      }
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}