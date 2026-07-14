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

  const size_t group_count = mins.size();
  const bool can_prune =
      block_size != 0 &&
      group_count != 0 &&
      group_count == maxs.size() &&
      (static_cast<size_t>(group_count - 1) * static_cast<size_t>(block_size) < n);

  if (!can_prune) {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t s = 0;
      const uint32_t* p = data;
      const uint32_t* end = data + n;
      for (; p != end; ++p) {
        const uint32_t v = *p;
        if (v >= low && v <= high) s += static_cast<uint64_t>(v);
      }
      sum = s;
    }
    return sum;
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    size_t offset = 0;

    for (size_t g = 0; g < group_count; ++g) {
      const uint32_t gmin = mins[g];
      const uint32_t gmax = maxs[g];

      if (gmax < low || gmin > high) {
        offset += block_size;
        continue;
      }

      size_t len = block_size;
      if (offset >= n) break;
      if (len > n - offset) len = n - offset;

      const uint32_t* p = data + offset;
      const uint32_t* end = p + len;

      if (gmin >= low && gmax <= high) {
        uint64_t local = 0;
        for (; p != end; ++p) {
          local += static_cast<uint64_t>(*p);
        }
        s += local;
      } else {
        uint64_t local = 0;
        for (; p != end; ++p) {
          const uint32_t v = *p;
          if (v >= low && v <= high) local += static_cast<uint64_t>(v);
        }
        s += local;
      }

      offset += block_size;
    }

    sum = s;
  }

  return sum;
}
