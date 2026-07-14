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
    size_t num_groups = mins.size();
    for (size_t i = 0; i < num_groups; ++i) {
      uint32_t group_min = mins[i];
      uint32_t group_max = maxs[i];
      
      // Skip group if it cannot contain values in [low, high]
      if (group_max < low || group_min > high) {
        continue;
      }
      
      // Determine group's value range in the values vector
      size_t start_idx = i * block_size;
      size_t end_idx = std::min(start_idx + block_size, values.size());
      
      // Sum values in this group that fall within [low, high]
      for (size_t j = start_idx; j < end_idx; ++j) {
        uint32_t value = values[j];
        if (value >= low && value <= high) {
          sum += static_cast<uint64_t>(value);
        }
      }
    }
  }
  return sum;
}