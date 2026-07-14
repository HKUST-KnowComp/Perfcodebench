#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  if (iters <= 0) return 0;
  uint64_t sum = 0;
  const size_t n = values.size();
  const size_t num_blocks = mins.size();
  for (size_t i = 0; i < num_blocks; ++i) {
    uint32_t block_min = mins[i];
    uint32_t block_max = maxs[i];
    // Block range does not intersect [low, high] -> skip entirely.
    if (low > block_max || high < block_min) continue;

    size_t start = i * block_size;
    size_t end = start + block_size;
    if (end > n) end = n;

    // Block is fully inside [low, high] -> sum whole block without per-value checks.
    if (block_min >= low && block_max <= high) {
      for (size_t j = start; j < end; ++j) {
        sum += values[j];
      }
    } else {
      // Partial overlap -> check each value.
      for (size_t j = start; j < end; ++j) {
        uint32_t v = values[j];
        if (v >= low && v <= high) sum += v;
      }
    }
  }
  return sum;
}