#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  // Precompute valid value ranges to scan once, reusing across all iterations
  std::vector<std::pair<size_t, size_t>> valid_ranges;
  const size_t total_values = values.size();
  const size_t num_blocks = mins.size();

  for (size_t b = 0; b < num_blocks; ++b) {
    const uint32_t block_min = mins[b];
    const uint32_t block_max = maxs[b];
    // Prune blocks entirely outside the query range
    if (block_max < low || block_min > high) {
      continue;
    }
    // Calculate bounds of this block in the values array, handle partial final block
    const size_t start = b * block_size;
    const size_t end = std::min(start + block_size, total_values);
    valid_ranges.emplace_back(start, end);
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    // Only scan elements in blocks that could contribute to the sum
    for (const auto& range : valid_ranges) {
      for (size_t i = range.first; i < range.second; ++i) {
        const uint32_t val = values[i];
        if (val >= low && val <= high) {
          sum += static_cast<uint64_t>(val);
        }
      }
    }
  }
  return sum;
}