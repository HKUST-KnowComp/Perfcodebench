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

  const std::size_t n = values.size();
  const uint32_t* data = values.data();

  // Fast path when metadata is unusable.
  const bool can_prune =
      block_size != 0 && !mins.empty() && mins.size() == maxs.size();

  uint64_t sum = 0;

  if (!can_prune) {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t local = 0;
      std::size_t i = 0;
      const std::size_t unroll_end = n & ~std::size_t(3);
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

  const std::size_t group_count = mins.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;

    for (std::size_t g = 0; g < group_count; ++g) {
      const uint32_t gmin = mins[g];
      const uint32_t gmax = maxs[g];

      if (gmax < low || gmin > high) continue;

      std::size_t start = g * static_cast<std::size_t>(block_size);
      if (start >= n) break;
      std::size_t end = start + static_cast<std::size_t>(block_size);
      if (end > n) end = n;

      const uint32_t* p = data + start;
      const std::size_t len = end - start;

      if (gmin >= low && gmax <= high) {
        std::size_t i = 0;
        const std::size_t unroll_end = len & ~std::size_t(3);
        for (; i < unroll_end; i += 4) {
          local += static_cast<uint64_t>(p[i + 0]);
          local += static_cast<uint64_t>(p[i + 1]);
          local += static_cast<uint64_t>(p[i + 2]);
          local += static_cast<uint64_t>(p[i + 3]);
        }
        for (; i < len; ++i) {
          local += static_cast<uint64_t>(p[i]);
        }
      } else {
        std::size_t i = 0;
        const std::size_t unroll_end = len & ~std::size_t(3);
        for (; i < unroll_end; i += 4) {
          const uint32_t v0 = p[i + 0];
          const uint32_t v1 = p[i + 1];
          const uint32_t v2 = p[i + 2];
          const uint32_t v3 = p[i + 3];
          if (v0 >= low && v0 <= high) local += static_cast<uint64_t>(v0);
          if (v1 >= low && v1 <= high) local += static_cast<uint64_t>(v1);
          if (v2 >= low && v2 <= high) local += static_cast<uint64_t>(v2);
          if (v3 >= low && v3 <= high) local += static_cast<uint64_t>(v3);
        }
        for (; i < len; ++i) {
          const uint32_t v = p[i];
          if (v >= low && v <= high) local += static_cast<uint64_t>(v);
        }
      }
    }

    sum = local;
  }

  return sum;
}
