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
  size_t num_blocks = values.size() / block_size;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (size_t i = 0; i < num_blocks; ++i) {
      uint32_t block_min = mins[i];
      uint32_t block_max = maxs[i];
      if (block_min > high || block_max < low) {
        continue;
      }
      size_t start = i * block_size;
      size_t end = start + block_size;
      for (size_t j = start; j < end; ++j) {
        uint32_t val = values[j];
        if (val >= low && val <= high) {
          sum += val;
        }
      }
    }
  }
  return sum;
}