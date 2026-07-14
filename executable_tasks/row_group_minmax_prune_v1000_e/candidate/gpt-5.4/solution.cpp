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
  if (values.empty()) return 0;
  if (low > high) return 0;

  const uint32_t* data = values.data();
  const size_t n = values.size();

  uint64_t sum = 0;

  // Use row-group metadata when it is structurally valid.
  const size_t group_count = mins.size();
  const bool can_prune =
      block_size != 0 &&
      group_count == maxs.size() &&
      group_count != 0 &&
      (static_cast<size_t>(group_count - 1) * static_cast<size_t>(block_size) < n) &&
      ((static_cast<size_t>(group_count - 1) * static_cast<size_t>(block_size) +
        static_cast<size_t>(block_size)) >= n);

  if (!can_prune) {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t local_sum = 0;
      size_t i = 0;
      const size_t unroll_end = n & ~static_cast<size_t>(3);
      for (; i < unroll_end; i += 4) {
        const uint32_t v0 = data[i + 0];
        const uint32_t v1 = data[i + 1];
        const uint32_t v2 = data[i + 2];
        const uint32_t v3 = data[i + 3];
        if (v0 >= low && v0 <= high) local_sum += static_cast<uint64_t>(v0);
        if (v1 >= low && v1 <= high) local_sum += static_cast<uint64_t>(v1);
        if (v2 >= low && v2 <= high) local_sum += static_cast<uint64_t>(v2);
        if (v3 >= low && v3 <= high) local_sum += static_cast<uint64_t>(v3);
      }
      for (; i < n; ++i) {
        const uint32_t v = data[i];
        if (v >= low && v <= high) local_sum += static_cast<uint64_t>(v);
      }
      sum = local_sum;
    }
    return sum;
  }

  const uint32_t* mins_data = mins.data();
  const uint32_t* maxs_data = maxs.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    size_t base = 0;

    for (size_t g = 0; g < group_count; ++g, base += block_size) {
      const uint32_t gmin = mins_data[g];
      const uint32_t gmax = maxs_data[g];

      // No overlap with predicate range: skip whole group.
      if (gmax < low || gmin > high) continue;

      size_t end = base + block_size;
      if (end > n) end = n;
      const uint32_t* p = data + base;
      const uint32_t* const pe = data + end;

      // Entire group is guaranteed in range: sum directly without per-value checks.
      if (gmin >= low && gmax <= high) {
        uint64_t group_sum = 0;
        const uint32_t* pu = p;
        const size_t len = static_cast<size_t>(pe - p);
        const size_t unroll = len & ~static_cast<size_t>(3);
        for (size_t i = 0; i < unroll; i += 4) {
          group_sum += static_cast<uint64_t>(pu[i + 0]);
          group_sum += static_cast<uint64_t>(pu[i + 1]);
          group_sum += static_cast<uint64_t>(pu[i + 2]);
          group_sum += static_cast<uint64_t>(pu[i + 3]);
        }
        for (size_t i = unroll; i < len; ++i) {
          group_sum += static_cast<uint64_t>(pu[i]);
        }
        local_sum += group_sum;
        continue;
      }

      // Partial overlap: filter within the group.
      const size_t len = static_cast<size_t>(pe - p);
      const size_t unroll = len & ~static_cast<size_t>(3);
      size_t i = 0;
      for (; i < unroll; i += 4) {
        const uint32_t v0 = p[i + 0];
        const uint32_t v1 = p[i + 1];
        const uint32_t v2 = p[i + 2];
        const uint32_t v3 = p[i + 3];
        if (v0 >= low && v0 <= high) local_sum += static_cast<uint64_t>(v0);
        if (v1 >= low && v1 <= high) local_sum += static_cast<uint64_t>(v1);
        if (v2 >= low && v2 <= high) local_sum += static_cast<uint64_t>(v2);
        if (v3 >= low && v3 <= high) local_sum += static_cast<uint64_t>(v3);
      }
      for (; i < len; ++i) {
        const uint32_t v = p[i];
        if (v >= low && v <= high) local_sum += static_cast<uint64_t>(v);
      }
    }

    sum = local_sum;
  }

  return sum;
}
