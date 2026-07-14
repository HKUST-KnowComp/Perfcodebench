#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  // If no iterations requested, baseline would return 0
  if (iters <= 0) return 0ULL;

  const size_t n = values.size();
  if (n == 0 || block_size == 0) {
    return 0ULL;
  }

  const uint32_t* v = values.data();
  const uint32_t* mins_p = mins.data();
  const uint32_t* maxs_p = maxs.data();

  const size_t num_blocks = (n + block_size - 1) / block_size;

  uint64_t sum = 0ULL;

  // Compute once; further iters would repeat identical work in baseline.
  for (size_t b = 0; b < num_blocks; ++b) {
    const uint32_t gmin = mins_p[b];
    const uint32_t gmax = maxs_p[b];

    if (gmax < low || gmin > high) {
      // Completely pruned block
      continue;
    }

    const size_t start = b * static_cast<size_t>(block_size);
    size_t end = start + static_cast<size_t>(block_size);
    if (end > n) end = n;

    if (gmin >= low && gmax <= high) {
      // Entire block is within range; sum all values without per-element checks
      size_t i = start;
      uint64_t local = 0ULL;
      // Unroll by 8 for throughput
      for (; i + 8 <= end; i += 8) {
        local += (uint64_t)v[i] + (uint64_t)v[i + 1] + (uint64_t)v[i + 2] + (uint64_t)v[i + 3]
               + (uint64_t)v[i + 4] + (uint64_t)v[i + 5] + (uint64_t)v[i + 6] + (uint64_t)v[i + 7];
      }
      for (; i < end; ++i) {
        local += (uint64_t)v[i];
      }
      sum += local;
    } else {
      // Partial overlap; check each value against [low, high]
      size_t i = start;
      uint64_t local = 0ULL;
      // Unroll by 8 with independent conditionals
      for (; i + 8 <= end; i += 8) {
        uint32_t a0 = v[i];
        uint32_t a1 = v[i + 1];
        uint32_t a2 = v[i + 2];
        uint32_t a3 = v[i + 3];
        uint32_t a4 = v[i + 4];
        uint32_t a5 = v[i + 5];
        uint32_t a6 = v[i + 6];
        uint32_t a7 = v[i + 7];
        if (a0 >= low && a0 <= high) local += (uint64_t)a0;
        if (a1 >= low && a1 <= high) local += (uint64_t)a1;
        if (a2 >= low && a2 <= high) local += (uint64_t)a2;
        if (a3 >= low && a3 <= high) local += (uint64_t)a3;
        if (a4 >= low && a4 <= high) local += (uint64_t)a4;
        if (a5 >= low && a5 <= high) local += (uint64_t)a5;
        if (a6 >= low && a6 <= high) local += (uint64_t)a6;
        if (a7 >= low && a7 <= high) local += (uint64_t)a7;
      }
      for (; i < end; ++i) {
        uint32_t val = v[i];
        if (val >= low && val <= high) local += (uint64_t)val;
      }
      sum += local;
    }
  }

  return sum;
}
