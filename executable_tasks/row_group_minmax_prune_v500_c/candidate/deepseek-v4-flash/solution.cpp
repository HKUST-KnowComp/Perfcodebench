#include "interface.h"
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
  size_t n = values.size();
  size_t blk = block_size;
  size_t nblocks = (n + blk - 1) / blk;
  uint32_t l = low, h = high;
  // Precompute list of blocks that can possibly contain values in [l, h]
  std::vector<uint32_t> relevant;
  for (size_t i = 0; i < nblocks; ++i) {
    if (maxs[i] >= l && mins[i] <= h) {
      relevant.push_back(static_cast<uint32_t>(i));
    }
  }
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (uint32_t bi : relevant) {
      size_t start = static_cast<size_t>(bi) * blk;
      size_t end = start + blk;
      if (end > n) end = n;
      for (size_t j = start; j < end; ++j) {
        uint32_t v = values[j];
        if (v >= l && v <= h) {
          sum += static_cast<uint64_t>(v);
        }
      }
    }
  }
  return sum;
}
