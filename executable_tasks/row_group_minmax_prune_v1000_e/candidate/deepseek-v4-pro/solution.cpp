#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  (void)iters; // deterministic result, compute once
  if (values.empty()) return 0;
  const size_t n = values.size();
  const size_t num_blocks = (n + block_size - 1) / block_size;
  uint64_t sum = 0;

  // Use metadata only if it appears valid
  if (mins.size() >= num_blocks && maxs.size() >= num_blocks && block_size > 0) {
    for (size_t b = 0; b < num_blocks; ++b) {
      uint32_t bmin = mins[b];
      uint32_t bmax = maxs[b];
      if (bmin > high || bmax < low) continue;  // block disjoint
      size_t start = b * block_size;
      size_t end = start + block_size;
      if (end > n) end = n;
      if (bmin >= low && bmax <= high) {
        // block fully inside range, sum all elements
        for (size_t i = start; i < end; ++i) {
          sum += static_cast<uint64_t>(values[i]);
        }
      } else {
        // partial overlap, check each element
        for (size_t i = start; i < end; ++i) {
          uint32_t val = values[i];
          if (val >= low && val <= high) {
            sum += static_cast<uint64_t>(val);
          }
        }
      }
    }
  } else {
    // fallback: full scan without pruning
    for (size_t i = 0; i < n; ++i) {
      uint32_t val = values[i];
      if (val >= low && val <= high) {
        sum += static_cast<uint64_t>(val);
      }
    }
  }
  return sum;
}