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
  size_t n = values.size();
  if (n == 0 || block_size == 0) {
    for (int iter = 0; iter < iters; ++iter) {
      sum = 0;
    }
    return sum;
  }
  size_t num_blocks = (n + block_size - 1) / block_size;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (size_t b = 0; b < num_blocks; ++b) {
      if (maxs[b] < low || mins[b] > high) {
        continue;
      }
      size_t start = b * block_size;
      size_t end = start + block_size;
      if (end > n) end = n;
      for (size_t i = start; i < end; ++i) {
        uint32_t val = values[i];
        if (val >= low && val <= high) {
          sum += static_cast<uint64_t>(val);
        }
      }
    }
  }
  return sum;
}
