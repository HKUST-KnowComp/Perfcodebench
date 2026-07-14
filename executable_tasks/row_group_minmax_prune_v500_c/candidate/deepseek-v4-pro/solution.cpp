#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  if (block_size == 0) return 0;

  const size_t n = values.size();
  const size_t num_blocks = (n + block_size - 1) / block_size;

  // fallback to full scan if metadata arrays do not match expected size
  if (mins.size() != num_blocks || maxs.size() != num_blocks) {
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
      sum = 0;
      const uint32_t* v = values.data();
      for (size_t i = 0; i < n; ++i) {
        uint32_t val = v[i];
        if (val >= low && val <= high) {
          sum += val;
        }
      }
    }
    return sum;
  }

  const uint32_t* v = values.data();
  const uint32_t* mmin = mins.data();
  const uint32_t* mmax = maxs.data();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (size_t b = 0; b < num_blocks; ++b) {
      size_t start = b * block_size;
      size_t end = start + block_size;
      if (end > n) end = n;

      uint32_t block_min = mmin[b];
      uint32_t block_max = mmax[b];

      // entire block outside range
      if (block_max < low || block_min > high) continue;

      // entire block fully inside range
      if (block_min >= low && block_max <= high) {
        for (size_t i = start; i < end; ++i) {
          sum += v[i];
        }
      } else {
        // partial overlap, per-value check
        for (size_t i = start; i < end; ++i) {
          uint32_t val = v[i];
          if (val >= low && val <= high) {
            sum += val;
          }
        }
      }
    }
  }
  return sum;
}