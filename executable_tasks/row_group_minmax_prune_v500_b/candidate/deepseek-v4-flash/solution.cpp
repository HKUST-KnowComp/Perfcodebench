#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  size_t n = values.size();
  size_t num_blocks = (n + block_size - 1) / block_size;
  
  // Precompute which blocks overlap the range [low, high]
  std::vector<size_t> block_starts;
  std::vector<size_t> block_ends;
  for (size_t b = 0; b < num_blocks; ++b) {
    if (mins[b] <= high && maxs[b] >= low) {
      size_t start = b * block_size;
      size_t end = start + block_size;
      if (end > n) end = n;
      block_starts.push_back(start);
      block_ends.push_back(end);
    }
  }
  
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (size_t i = 0; i < block_starts.size(); ++i) {
      size_t start = block_starts[i];
      size_t end = block_ends[i];
      for (size_t j = start; j < end; ++j) {
        uint32_t val = values[j];
        if (val >= low && val <= high) {
          sum += static_cast<uint64_t>(val);
        }
      }
    }
  }
  return sum;
}
