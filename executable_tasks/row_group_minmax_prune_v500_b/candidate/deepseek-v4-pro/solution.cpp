#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  if (iters <= 0) return 0;

  const size_t num_values = values.size();
  const size_t num_blocks = mins.size();
  std::vector<uint64_t> block_sums(num_blocks);
  std::vector<int> block_type(num_blocks); // 0 = skip, 1 = full, 2 = partial

  // Precompute block sums and classify each block once
  for (size_t i = 0; i < num_blocks; ++i) {
    size_t start = i * block_size;
    size_t end = std::min(start + block_size, num_values);
    uint64_t bsum = 0;
    for (size_t j = start; j < end; ++j) {
      bsum += values[j];
    }
    block_sums[i] = bsum;

    uint32_t bmin = mins[i];
    uint32_t bmax = maxs[i];
    if (bmax < low || bmin > high) {
      block_type[i] = 0; // skip
    } else if (bmin >= low && bmax <= high) {
      block_type[i] = 1; // full
    } else {
      block_type[i] = 2; // partial
    }
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (size_t i = 0; i < num_blocks; ++i) {
      switch (block_type[i]) {
        case 1: // fully inside range
          sum += block_sums[i];
          break;
        case 2: { // partial overlap
          size_t start = i * block_size;
          size_t end = std::min(start + block_size, num_values);
          for (size_t j = start; j < end; ++j) {
            uint32_t v = values[j];
            if (v >= low && v <= high) {
              sum += v;
            }
          }
          break;
        }
        default: // skip block
          break;
      }
    }
  }
  return sum;
}
