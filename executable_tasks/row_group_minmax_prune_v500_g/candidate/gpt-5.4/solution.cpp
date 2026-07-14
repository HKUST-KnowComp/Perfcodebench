#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  const size_t n = values.size();
  if (iters <= 0 || n == 0) return 0;

  // Invalid range: no value can match.
  if (low > high) return 0;

  // Degenerate block size fallback.
  if (block_size == 0) {
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t s = 0;
      const uint32_t* p = values.data();
      const uint32_t* end = p + n;
      while (p != end) {
        const uint32_t v = *p++;
        if (v >= low && v <= high) s += static_cast<uint64_t>(v);
      }
      sum = s;
    }
    return sum;
  }

  const uint32_t* vals = values.data();
  const size_t num_blocks = (n + static_cast<size_t>(block_size) - 1) / static_cast<size_t>(block_size);

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;

    size_t base = 0;
    for (size_t b = 0; b < num_blocks; ++b, base += block_size) {
      const size_t remaining = n - base;
      const size_t len = remaining < static_cast<size_t>(block_size)
                             ? remaining
                             : static_cast<size_t>(block_size);

      // If metadata is unavailable for this block, conservatively scan.
      if (b >= mins.size() || b >= maxs.size()) {
        const uint32_t* p = vals + base;
        const uint32_t* end = p + len;
        while (p != end) {
          const uint32_t v = *p++;
          if (v >= low && v <= high) s += static_cast<uint64_t>(v);
        }
        continue;
      }

      const uint32_t bmin = mins[b];
      const uint32_t bmax = maxs[b];

      // Entire block excluded.
      if (bmax < low || bmin > high) {
        continue;
      }

      const uint32_t* p = vals + base;
      const uint32_t* end = p + len;

      // Entire block included.
      if (bmin >= low && bmax <= high) {
        uint64_t block_sum = 0;
        while (p != end) {
          block_sum += static_cast<uint64_t>(*p++);
        }
        s += block_sum;
        continue;
      }

      // Partial overlap: filter within block.
      while (p != end) {
        const uint32_t v = *p++;
        if (v >= low && v <= high) s += static_cast<uint64_t>(v);
      }
    }

    sum = s;
  }

  return sum;
}
