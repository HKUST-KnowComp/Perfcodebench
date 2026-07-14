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
  const uint32_t* vals = values.data();

  uint64_t sum = 0;

  // Fast path when metadata is unusable.
  if (block_size == 0 || mins.empty() || maxs.empty()) {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t s = 0;
      std::size_t i = 0;
      const std::size_t unroll_end = n & ~std::size_t(3);
      for (; i < unroll_end; i += 4) {
        const uint32_t v0 = vals[i + 0];
        const uint32_t v1 = vals[i + 1];
        const uint32_t v2 = vals[i + 2];
        const uint32_t v3 = vals[i + 3];
        if (v0 >= low && v0 <= high) s += static_cast<uint64_t>(v0);
        if (v1 >= low && v1 <= high) s += static_cast<uint64_t>(v1);
        if (v2 >= low && v2 <= high) s += static_cast<uint64_t>(v2);
        if (v3 >= low && v3 <= high) s += static_cast<uint64_t>(v3);
      }
      for (; i < n; ++i) {
        const uint32_t v = vals[i];
        if (v >= low && v <= high) s += static_cast<uint64_t>(v);
      }
      sum = s;
    }
    return sum;
  }

  const std::size_t num_blocks_by_size = (n + static_cast<std::size_t>(block_size) - 1) / static_cast<std::size_t>(block_size);
  std::size_t num_blocks = mins.size();
  if (maxs.size() < num_blocks) num_blocks = maxs.size();
  if (num_blocks_by_size < num_blocks) num_blocks = num_blocks_by_size;

  if (num_blocks == 0) {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t s = 0;
      std::size_t i = 0;
      const std::size_t unroll_end = n & ~std::size_t(3);
      for (; i < unroll_end; i += 4) {
        const uint32_t v0 = vals[i + 0];
        const uint32_t v1 = vals[i + 1];
        const uint32_t v2 = vals[i + 2];
        const uint32_t v3 = vals[i + 3];
        if (v0 >= low && v0 <= high) s += static_cast<uint64_t>(v0);
        if (v1 >= low && v1 <= high) s += static_cast<uint64_t>(v1);
        if (v2 >= low && v2 <= high) s += static_cast<uint64_t>(v2);
        if (v3 >= low && v3 <= high) s += static_cast<uint64_t>(v3);
      }
      for (; i < n; ++i) {
        const uint32_t v = vals[i];
        if (v >= low && v <= high) s += static_cast<uint64_t>(v);
      }
      sum = s;
    }
    return sum;
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    std::size_t base = 0;

    for (std::size_t b = 0; b < num_blocks; ++b) {
      const uint32_t bmin = mins[b];
      const uint32_t bmax = maxs[b];

      if (bmax < low || bmin > high) {
        base += block_size;
        continue;
      }

      std::size_t end = base + block_size;
      if (end > n) end = n;

      std::size_t i = base;
      const std::size_t len = end - base;
      const std::size_t unroll_end = base + (len & ~std::size_t(3));
      for (; i < unroll_end; i += 4) {
        const uint32_t v0 = vals[i + 0];
        const uint32_t v1 = vals[i + 1];
        const uint32_t v2 = vals[i + 2];
        const uint32_t v3 = vals[i + 3];
        if (v0 >= low && v0 <= high) s += static_cast<uint64_t>(v0);
        if (v1 >= low && v1 <= high) s += static_cast<uint64_t>(v1);
        if (v2 >= low && v2 <= high) s += static_cast<uint64_t>(v2);
        if (v3 >= low && v3 <= high) s += static_cast<uint64_t>(v3);
      }
      for (; i < end; ++i) {
        const uint32_t v = vals[i];
        if (v >= low && v <= high) s += static_cast<uint64_t>(v);
      }

      base = end;
    }

    // If metadata covered fewer blocks than implied by values size, scan the tail.
    for (std::size_t i = base; i < n; ++i) {
      const uint32_t v = vals[i];
      if (v >= low && v <= high) s += static_cast<uint64_t>(v);
    }

    sum = s;
  }

  return sum;
}
