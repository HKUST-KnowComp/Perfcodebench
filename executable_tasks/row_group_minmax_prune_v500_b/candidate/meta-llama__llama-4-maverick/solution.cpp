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
        if (mins[i] >= low && maxs[i] <= high) {
          sum += static_cast<uint64_t>(block_size) * (mins[i] + maxs[i]) / 2;
        } else {
          for (size_t j = i * block_size; j < (i + 1) * block_size && j < values.size(); ++j) {
            if (values[j] >= low && values[j] <= high) {
              sum += static_cast<uint64_t>(values[j]);
            }
          }
        }
      }
    }
  }
  return sum;
}