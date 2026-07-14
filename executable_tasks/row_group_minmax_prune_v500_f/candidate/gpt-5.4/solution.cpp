#include "interface.h"

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

  // Preserve full-scan semantics for invalid/degenerate metadata inputs.
  const bool metadata_usable =
      block_size != 0 && mins.size() == maxs.size() &&
      mins.size() == (n + static_cast<size_t>(block_size) - 1) / static_cast<size_t>(block_size);

  uint64_t sum = 0;

  if (!metadata_usable) {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t local = 0;
      const uint32_t* p = values.data();
      const uint32_t* end = p + n;
      while (p != end) {
        const uint32_t v = *p++;
        if (v >= low && v <= high) local += static_cast<uint64_t>(v);
      }
      sum = local;
    }
    return sum;
  }

  const uint32_t* vals = values.data();
  const uint32_t* minp = mins.data();
  const uint32_t* maxp = maxs.data();
  const size_t num_groups = mins.size();
  const size_t bs = static_cast<size_t>(block_size);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;
    size_t base = 0;

    for (size_t g = 0; g < num_groups; ++g, base += bs) {
      const uint32_t gmin = minp[g];
      const uint32_t gmax = maxp[g];

      if (gmax < low || gmin > high) continue;

      const size_t remaining = n - base;
      const size_t len = remaining < bs ? remaining : bs;
      const uint32_t* p = vals + base;
      const uint32_t* end = p + len;

      if (gmin >= low && gmax <= high) {
        uint64_t block_sum = 0;
        while (p != end) {
          block_sum += static_cast<uint64_t>(*p++);
        }
        local += block_sum;
      } else {
        while (p != end) {
          const uint32_t v = *p++;
          if (v >= low && v <= high) local += static_cast<uint64_t>(v);
        }
      }
    }

    sum = local;
  }

  return sum;
}
