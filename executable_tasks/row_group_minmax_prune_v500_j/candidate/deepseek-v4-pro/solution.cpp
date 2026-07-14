#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  const size_t n = values.size();
  const uint32_t* val_ptr = values.data();
  const uint32_t* min_ptr = mins.data();
  const uint32_t* max_ptr = maxs.data();
  const size_t n_blocks = (n + block_size - 1) / block_size;

  uint64_t final_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    for (size_t b = 0; b < n_blocks; ++b) {
      size_t start = b * block_size;
      size_t end = start + block_size;
      if (end > n) end = n;

      uint32_t block_min = min_ptr[b];
      uint32_t block_max = max_ptr[b];

      // Entire block is outside the range
      if (block_max < low || block_min > high) {
        continue;
      }

      // Entire block is inside the range
      if (block_min >= low && block_max <= high) {
        for (size_t i = start; i < end; ++i) {
          sum += val_ptr[i];
        }
      } else {
        // Partial overlap: check each element
        for (size_t i = start; i < end; ++i) {
          uint32_t v = val_ptr[i];
          if (v >= low && v <= high) {
            sum += v;
          }
        }
      }
    }
    final_sum = sum;
  }
  return final_sum;
}