#include "interface.h"
#include <algorithm>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  if (values.empty() || block_size == 0 || mins.empty() || maxs.empty()) return 0;
  
  const size_t total_values = values.size();
  const size_t num_blocks = (total_values + block_size - 1) / block_size;
  uint64_t final_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    for (size_t b = 0; b < num_blocks; ++b) {
      // Prune blocks that are entirely outside the query range
      if (maxs[b] < low || mins[b] > high) {
        continue;
      }
      // Only scan values in overlapping blocks
      const size_t block_start = b * block_size;
      const size_t block_end = std::min(block_start + block_size, total_values);
      for (size_t i = block_start; i < block_end; ++i) {
        const uint32_t v = values[i];
        if (v >= low && v <= high) {
          sum += static_cast<uint64_t>(v);
        }
      }
    }
    final_sum = sum;
  }
  return final_sum;
}