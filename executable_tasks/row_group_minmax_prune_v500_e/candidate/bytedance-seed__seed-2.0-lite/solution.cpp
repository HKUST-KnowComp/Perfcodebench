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
    const size_t num_blocks = mins.size();
    for (size_t i = 0; i < num_blocks; ++i) {
      // Prune entire block if it lies fully outside the query range
      if (maxs[i] < low || mins[i] > high) {
        continue;
      }
      // Calculate valid bounds for the current block to avoid out-of-bounds access
      const uint32_t start = static_cast<uint32_t>(i) * block_size;
      uint32_t end = start + block_size;
      if (end > values.size()) {
        end = static_cast<uint32_t>(values.size());
      }
      // Scan only the overlapping block to accumulate qualifying values
      for (uint32_t j = start; j < end; ++j) {
        const uint32_t value = values[j];
        if (value >= low && value <= high) {
          sum += static_cast<uint64_t>(value);
        }
      }
    }
  }
  return sum;
}