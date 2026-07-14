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
  const uint32_t n = static_cast<uint32_t>(values.size());
  const uint32_t num_blocks = static_cast<uint32_t>((n + block_size - 1) / block_size);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (uint32_t b = 0; b < num_blocks; ++b) {
      uint32_t b_min = mins[b];
      uint32_t b_max = maxs[b];

      // Case 1: Block is entirely outside the range
      if (b_max < low || b_min > high) {
        continue;
      }

      // Case 2: Block is entirely within the range
      if (b_min >= low && b_max <= high) {
        uint32_t start = b * block_size;
        uint32_t end = std::min(start + block_size, n);
        for (uint32_t i = start; i < end; ++i) {
          current_iter_sum += static_cast<uint64_t>(values[i]);
        }
      }
      // Case 3: Block partially overlaps the range
      else {
        uint32_t start = b * block_size;
        uint32_t end = std::min(start + block_size, n);
        for (uint32_t i = start; i < end; ++i) {
          uint32_t val = values[i];
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