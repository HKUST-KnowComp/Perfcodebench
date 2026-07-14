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
  const size_t n = values.size();
  const size_t num_blocks = mins.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < num_blocks; ++i) {
      uint32_t b_min = mins[i];
      uint32_t b_max = maxs[i];

      // Pruning: If the block's range is completely outside [low, high], skip it.
      if (b_max < low || b_min > high) {
        continue;
      }

      size_t start_idx = i * block_size;
      size_t end_idx = std::min(start_idx + block_size, n);

      // Optimization: If the block's range is entirely within [low, high], sum all elements.
      if (b_min >= low && b_max <= high) {
        for (size_t j = start_idx; j < end_idx; ++j) {
          current_iter_sum += static_cast<uint64_t>(values[j]);
        }
      } else {
        // Partial overlap: Scan elements individually.
        for (size_t j = start_idx; j < end_idx; ++j) {
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