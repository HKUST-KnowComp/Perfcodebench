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
  const uint32_t n = static_cast<uint32_t>(values.size());
  const uint32_t num_blocks = (n + block_size - 1) / block_size;
  const uint32_t* vptr = values.data();
  const uint32_t* minptr = mins.data();
  const uint32_t* maxptr = maxs.data();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (uint32_t b = 0; b < num_blocks; ++b) {
      const uint32_t bmin = minptr[b];
      const uint32_t bmax = maxptr[b];
      if (bmax < low || bmin > high) {
        continue;               // skip entire block
      }
      uint32_t start = b * block_size;
      uint32_t end = start + block_size;
      if (end > n) end = n;
      for (uint32_t i = start; i < end; ++i) {
        const uint32_t v = vptr[i];
        if (v >= low && v <= high) {
          sum += static_cast<uint64_t>(v);
        }
      }
    }
  }
  return sum;
}
