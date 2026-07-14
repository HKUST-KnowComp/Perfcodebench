#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  (void)iters;
  uint64_t sum = 0;
  size_t n = values.size();
  if (n == 0) return 0;
  size_t nb = (n + block_size - 1) / block_size;
  for (size_t b = 0; b < nb; ++b) {
    uint32_t bmin = mins[b];
    uint32_t bmax = maxs[b];
    if (bmax < low || bmin > high) continue;
    size_t start = b * block_size;
    size_t end = start + block_size;
    if (end > n) end = n;
    if (bmin >= low && bmax <= high) {
      for (size_t i = start; i < end; ++i)
        sum += values[i];
    } else {
      for (size_t i = start; i < end; ++i) {
        uint32_t v = values[i];
        if (v >= low && v <= high)
          sum += v;
      }
    }
  }
  return sum;
}