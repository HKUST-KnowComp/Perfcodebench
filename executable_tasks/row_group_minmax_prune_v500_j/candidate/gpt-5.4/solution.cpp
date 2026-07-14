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
  if (iters <= 0) return 0;
  if (values.empty()) return 0;
  if (low > high) return 0;

  const uint32_t* data = values.data();
  const std::size_t n = values.size();

  uint64_t sum = 0;

  // Fast path when row-group metadata is unusable.
  if (block_size == 0 || mins.empty() || maxs.empty()) {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t local = 0;
      const uint32_t* p = data;
      const uint32_t* end = data + n;
      while (p != end) {
        const uint32_t v = *p++;
        if (v >= low && v <= high) local += static_cast<uint64_t>(v);
      }
      sum = local;
    }
    return sum;
  }

  const std::size_t group_count_by_values = (n + static_cast<std::size_t>(block_size) - 1) /
                                            static_cast<std::size_t>(block_size);
  std::size_t group_count = mins.size();
  if (maxs.size() < group_count) group_count = maxs.size();
  if (group_count_by_values < group_count) group_count = group_count_by_values;

  // If metadata coverage is absent or inconsistent, fall back to full scan.
  if (group_count == 0) {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t local = 0;
      const uint32_t* p = data;
      const uint32_t* end = data + n;
      while (p != end) {
        const uint32_t v = *p++;
        if (v >= low && v <= high) local += static_cast<uint64_t>(v);
      }
      sum = local;
    }
    return sum;
  }

  const uint32_t* mins_data = mins.data();
  const uint32_t* maxs_data = maxs.data();
  const std::size_t bs = static_cast<std::size_t>(block_size);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;
    std::size_t base = 0;

    for (std::size_t g = 0; g < group_count; ++g) {
      const uint32_t gmin = mins_data[g];
      const uint32_t gmax = maxs_data[g];

      if (gmax < low || gmin > high) {
        base += bs;
        continue;
      }

      std::size_t end_idx = base + bs;
      if (end_idx > n) end_idx = n;

      const uint32_t* p = data + base;
      const uint32_t* end = data + end_idx;
      while (p != end) {
        const uint32_t v = *p++;
        if (v >= low && v <= high) local += static_cast<uint64_t>(v);
      }

      base += bs;
      if (base >= n) break;
    }

    // Scan any tail not covered by metadata to preserve correctness.
    if (base < n) {
      const uint32_t* p = data + base;
      const uint32_t* end = data + n;
      while (p != end) {
        const uint32_t v = *p++;
        if (v >= low && v <= high) local += static_cast<uint64_t>(v);
      }
    }

    sum = local;
  }

  return sum;
}
