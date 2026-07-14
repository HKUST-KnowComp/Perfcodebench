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
  if (iters <= 0) return 0;

  const uint32_t* vals = values.empty() ? nullptr : values.data();
  const uint32_t* minp = mins.empty() ? nullptr : mins.data();
  const uint32_t* maxp = maxs.empty() ? nullptr : maxs.data();
  const size_t n = values.size();

  uint64_t sum = 0;

  // If the range is invalid, result is always zero.
  if (low > high) {
    return 0;
  }

  // Use row-group metadata when it is plausibly valid.
  const bool can_prune =
      block_size != 0 && minp != nullptr && maxp != nullptr &&
      mins.size() == maxs.size() &&
      mins.size() >= (n + static_cast<size_t>(block_size) - 1) / static_cast<size_t>(block_size);

  if (!can_prune) {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t local = 0;
      const uint32_t* p = vals;
      const uint32_t* end = vals + n;
      while (p != end) {
        const uint32_t v = *p++;
        if (v >= low && v <= high) local += static_cast<uint64_t>(v);
      }
      sum = local;
    }
    return sum;
  }

  const size_t bs = static_cast<size_t>(block_size);
  const size_t num_blocks = (n + bs - 1) / bs;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;

    size_t base = 0;
    for (size_t b = 0; b < num_blocks; ++b, base += bs) {
      const uint32_t bmin = minp[b];
      const uint32_t bmax = maxp[b];

      // No overlap with query range: skip whole block.
      if (bmax < low || bmin > high) continue;

      const size_t end_idx = (base + bs < n) ? (base + bs) : n;
      const uint32_t* p = vals + base;
      const uint32_t* end = vals + end_idx;

      while (p != end) {
        const uint32_t v = *p++;
        if (v >= low && v <= high) local += static_cast<uint64_t>(v);
      }
    }

    sum = local;
  }

  return sum;
}
