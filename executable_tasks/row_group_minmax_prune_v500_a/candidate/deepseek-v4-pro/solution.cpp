#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  if (iters <= 0) return 0;
  uint64_t sum = 0;
  size_t n = values.size();
  if (n == 0) return sum;
  size_t num_blocks = n / block_size;
  for (size_t b = 0; b < num_blocks; ++b) {
    if (maxs[b] < low || mins[b] > high) continue;
    size_t start = b * block_size;
    size_t end = start + block_size;
    if (mins[b] >= low && maxs[b] <= high) {
      for (size_t i = start; i < end; ++i) {
        sum += values[i];
      }
    } else {
      for (size_t i = start; i < end; ++i) {
        uint32_t v = values[i];
        if (v >= low && v <= high) sum += v;
      }
    }
  }
  size_t remainder_start = num_blocks * block_size;
  for (size_t i = remainder_start; i < n; ++i) {
    uint32_t v = values[i];
    if (v >= low && v <= high) sum += v;
  }
  return sum;
}
