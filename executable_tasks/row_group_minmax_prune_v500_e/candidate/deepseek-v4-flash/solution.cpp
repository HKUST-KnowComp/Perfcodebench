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
  uint32_t num_blocks = static_cast<uint32_t>(values.size() / block_size);

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (uint32_t b = 0; b < num_blocks; ++b) {
      uint32_t block_min = mins[b];
      uint32_t block_max = maxs[b];

      // If block does not intersect [low, high], skip entire block
      if (block_max < low || block_min > high) continue;

      uint32_t start = b * block_size;
      uint32_t end = start + block_size;

      // If entire block lies inside the range, sum without per-element check
      if (block_min >= low && block_max <= high) {
        for (uint32_t i = start; i < end; ++i) {
          sum += values[i];
        }
      } else {
        // Partial overlap: check each element individually
        for (uint32_t i = start; i < end; ++i) {
          uint32_t v = values[i];
          if (v >= low && v <= high) {
            sum += v;
          }
        }
      }
    }
  }
  return sum;
}
