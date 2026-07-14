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
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (size_t i = 0; i < mins.size(); ++i) {
      if (mins[i] <= high && maxs[i] >= low) {
        uint32_t start_idx = i * block_size;
        uint32_t end_idx = std::min(start_idx + block_size, static_cast<uint32_t>(values.size()));
        for (uint32_t j = start_idx; j < end_idx; ++j) {
          if (values[j] >= low && values[j] <= high) {
            sum += static_cast<uint64_t>(values[j]);
          }
        }
      }
    }
  }
  return sum;
}