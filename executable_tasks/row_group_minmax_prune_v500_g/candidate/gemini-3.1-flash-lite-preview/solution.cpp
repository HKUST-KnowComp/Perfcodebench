#include "interface.h"
#include <algorithm>

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
    for (size_t b = 0; b < num_blocks; ++b) {
      uint32_t min_val = mins[b];
      uint32_t max_val = maxs[b];

      // Prune: block is completely outside the range
      if (max_val < low || min_val > high) {
        continue;
      }

      size_t start = b * block_size;
      size_t end = std::min(start + block_size, n);

      // Optimization: If block is completely inside, sum all elements
      if (min_val >= low && max_val <= high) {
        for (size_t i = start; i < end; ++i) {
          current_iter_sum += values[i];
        }
      } else {
        // Partial overlap: check individual elements
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