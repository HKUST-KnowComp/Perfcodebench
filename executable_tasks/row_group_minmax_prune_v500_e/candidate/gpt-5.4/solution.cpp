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

  const size_t n = values.size();
  const uint32_t* data = values.data();

  uint64_t sum = 0;

  // Use row-group metadata when it is structurally valid.
  const bool can_prune =
      block_size != 0 && !mins.empty() && mins.size() == maxs.size();

  if (!can_prune) {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t local = 0;
      size_t i = 0;
      const size_t unroll_end = n & ~static_cast<size_t>(3);
      for (; i < unroll_end; i += 4) {
        const uint32_t v0 = data[i + 0];
        const uint32_t v1 = data[i + 1];
        const uint32_t v2 = data[i + 2];
        const uint32_t v3 = data[i + 3];
        if (v0 >= low && v0 <= high) local += static_cast<uint64_t>(v0);
        if (v1 >= low && v1 <= high) local += static_cast<uint64_t>(v1);
        if (v2 >= low && v2 <= high) local += static_cast<uint64_t>(v2);
        if (v3 >= low && v3 <= high) local += static_cast<uint64_t>(v3);
      }
      for (; i < n; ++i) {
        const uint32_t v = data[i];
        if (v >= low && v <= high) local += static_cast<uint64_t>(v);
      }
      sum = local;
    }
    return sum;
  }

  const size_t bs = static_cast<size_t>(block_size);
  const size_t group_count = mins.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;

    size_t base = 0;
    for (size_t g = 0; g < group_count && base < n; ++g, base += bs) {
      const uint32_t gmin = mins[g];
      const uint32_t gmax = maxs[g];

      // No overlap with query range: prune whole group.
      if (gmax < low || gmin > high) continue;

      const size_t end = (base + bs < n) ? (base + bs) : n;
      const uint32_t* p = data + base;
      const size_t len = end - base;

      // Full containment: all values in the group qualify.
      if (gmin >= low && gmax <= high) {
        uint64_t group_sum = 0;
        size_t j = 0;
        const size_t unroll_end = len & ~static_cast<size_t>(7);
        for (; j < unroll_end; j += 8) {
          group_sum += static_cast<uint64_t>(p[j + 0]);
          group_sum += static_cast<uint64_t>(p[j + 1]);
          group_sum += static_cast<uint64_t>(p[j + 2]);
          group_sum += static_cast<uint64_t>(p[j + 3]);
          group_sum += static_cast<uint64_t>(p[j + 4]);
          group_sum += static_cast<uint64_t>(p[j + 5]);
          group_sum += static_cast<uint64_t>(p[j + 6]);
          group_sum += static_cast<uint64_t>(p[j + 7]);
        }
        for (; j < len; ++j) {
          group_sum += static_cast<uint64_t>(p[j]);
        }
        local += group_sum;
        continue;
      }

      // Partial overlap: filter within the group.
      size_t j = 0;
      const size_t unroll_end = len & ~static_cast<size_t>(3);
      for (; j < unroll_end; j += 4) {
        const uint32_t v0 = p[j + 0];
        const uint32_t v1 = p[j + 1];
        const uint32_t v2 = p[j + 2];
        const uint32_t v3 = p[j + 3];
        if (v0 >= low && v0 <= high) local += static_cast<uint64_t>(v0);
        if (v1 >= low && v1 <= high) local += static_cast<uint64_t>(v1);
        if (v2 >= low && v2 <= high) local += static_cast<uint64_t>(v2);
        if (v3 >= low && v3 <= high) local += static_cast<uint64_t>(v3);
      }
      for (; j < len; ++j) {
        const uint32_t v = p[j];
        if (v >= low && v <= high) local += static_cast<uint64_t>(v);
      }
    }

    // If metadata covered fewer groups than needed, conservatively scan tail.
    if (base < n) {
      size_t i = base;
      const size_t rem = n - i;
      size_t j = 0;
      const uint32_t* p = data + i;
      const size_t unroll_end = rem & ~static_cast<size_t>(3);
      for (; j < unroll_end; j += 4) {
        const uint32_t v0 = p[j + 0];
        const uint32_t v1 = p[j + 1];
        const uint32_t v2 = p[j + 2];
        const uint32_t v3 = p[j + 3];
        if (v0 >= low && v0 <= high) local += static_cast<uint64_t>(v0);
        if (v1 >= low && v1 <= high) local += static_cast<uint64_t>(v1);
        if (v2 >= low && v2 <= high) local += static_cast<uint64_t>(v2);
        if (v3 >= low && v3 <= high) local += static_cast<uint64_t>(v3);
      }
      for (; j < rem; ++j) {
        const uint32_t v = p[j];
        if (v >= low && v <= high) local += static_cast<uint64_t>(v);
      }
    }

    sum = local;
  }

  return sum;
}
