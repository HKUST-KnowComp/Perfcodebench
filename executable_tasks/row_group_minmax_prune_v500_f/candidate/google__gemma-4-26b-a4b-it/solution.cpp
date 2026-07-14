#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  uint64_t total_sum = 0;
  const size_t num_groups = mins.size();
  const size_t num_values = values.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < num_groups; ++i) {
      uint32_t g_min = mins[i];
      uint32_t g_max = maxs[i];

      // Case 1: Group is completely outside the range
      if (g_max < low || g_min > high) {
        continue;
      }

      // Case 2: Group is completely inside the range
      if (g_min >= low && g_max <= high) {
        size_t start = i * block_size;
        size_t end = (i + 1) * block_size;
        if (end > num_values) end = num_values;
        
        for (size_t j = start; j < end; ++j) {
          current_iter_sum += static_cast<uint64_t>(values[j]);
        }
      }
      // Case 3: Group partially overlaps the range
      else {
        size_t start = i * block_size;
        size_t end = (i + 1) * block_size;
        if (end > num_values) end = num_values;

        for (size_t j = start; j < end; ++j) {
          uint32_t val = values[j];
          if (val >= low && val <= high) {
            current_iter_sum += static_cast<uint64_t>(val);
          }
        }
      }
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}