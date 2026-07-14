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
  const std::size_t blocks = mins.size();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t b = 0; b < blocks; ++b) {
      if (maxs[b] < low || mins[b] > high) {
        continue;
      }
      const std::size_t start = b * static_cast<std::size_t>(block_size);
      for (uint32_t i = 0; i < block_size; ++i) {
        const uint32_t value = values[start + static_cast<std::size_t>(i)];
        if (value >= low && value <= high) {
          sum += static_cast<uint64_t>(value);
        }
      }
    }
  }
  return sum;
}
