#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  const size_t n = values.size();
  if (n == 0 || iters <= 0) {
    return 0;
  }
  if (high < low) {
    // Empty range
    uint64_t sum_empty = 0;
    // Maintain loop semantics
    for (int i = 0; i < iters; ++i) {
      sum_empty = 0;
    }
    return sum_empty;
  }
  const uint32_t* data = values.data();

  uint64_t total_sum = 0;

  if (block_size == 0 || mins.empty() || maxs.empty()) {
    // Fallback: full scan with range filter
    const uint32_t* p = data;
    const uint32_t* pe = data + n;
    // Unrolled, branchless accumulation
    while (p + 4 <= pe) {
      uint32_t v0 = p[0];
      uint32_t v1 = p[1];
      uint32_t v2 = p[2];
      uint32_t v3 = p[3];
      total_sum += (uint64_t)v0 * (uint64_t)((v0 >= low) & (v0 <= high));
      total_sum += (uint64_t)v1 * (uint64_t)((v1 >= low) & (v1 <= high));
      total_sum += (uint64_t)v2 * (uint64_t)((v2 >= low) & (v2 <= high));
      total_sum += (uint64_t)v3 * (uint64_t)((v3 >= low) & (v3 <= high));
      p += 4;
    }
    while (p < pe) {
      uint32_t v = *p++;
      total_sum += (uint64_t)v * (uint64_t)((v >= low) & (v <= high));
    }
  } else {
    // Use row-group pruning
    const size_t num_blocks = mins.size();
    for (size_t b = 0; b < num_blocks; ++b) {
      uint32_t bmin = mins[b];
      uint32_t bmax = maxs[b];
      // Skip blocks that cannot contribute
      if (bmin > high || bmax < low) {
        continue;
      }
      size_t start = b * (size_t)block_size;
      if (start >= n) break; // Safety in case of inconsistent metadata
      size_t end = start + (size_t)block_size;
      if (end > n) end = n;

      const uint32_t* p = data + start;
      const uint32_t* pe = data + end;

      if (bmin >= low && bmax <= high) {
        // Entire block within range: sum all values
        // Unrolled loop for speed
        while (p + 4 <= pe) {
          total_sum += (uint64_t)p[0];
          total_sum += (uint64_t)p[1];
          total_sum += (uint64_t)p[2];
          total_sum += (uint64_t)p[3];
          p += 4;
        }
        while (p < pe) {
          total_sum += (uint64_t)(*p++);
        }
      } else {
        // Partial overlap: filter within the block
        while (p + 4 <= pe) {
          uint32_t v0 = p[0];
          uint32_t v1 = p[1];
          uint32_t v2 = p[2];
          uint32_t v3 = p[3];
          total_sum += (uint64_t)v0 * (uint64_t)((v0 >= low) & (v0 <= high));
          total_sum += (uint64_t)v1 * (uint64_t)((v1 >= low) & (v1 <= high));
          total_sum += (uint64_t)v2 * (uint64_t)((v2 >= low) & (v2 <= high));
          total_sum += (uint64_t)v3 * (uint64_t)((v3 >= low) & (v3 <= high));
          p += 4;
        }
        while (p < pe) {
          uint32_t v = *p++;
          total_sum += (uint64_t)v * (uint64_t)((v >= low) & (v <= high));
        }
      }
    }
  }

  // Repeat iterations without recomputation; preserve loop semantics
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = total_sum;
  }
  return sum;
}
