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

  const bool can_prune =
      block_size != 0 && !mins.empty() && mins.size() == maxs.size();

  if (!can_prune) {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t local = 0;
      const uint32_t* p = data;
      const uint32_t* end = data + n;

      while (p + 4 <= end) {
        uint32_t v0 = p[0];
        uint32_t v1 = p[1];
        uint32_t v2 = p[2];
        uint32_t v3 = p[3];
        if (v0 >= low && v0 <= high) local += v0;
        if (v1 >= low && v1 <= high) local += v1;
        if (v2 >= low && v2 <= high) local += v2;
        if (v3 >= low && v3 <= high) local += v3;
        p += 4;
      }
      while (p < end) {
        uint32_t v = *p++;
        if (v >= low && v <= high) local += v;
      }
      sum = local;
    }
    return sum;
  }

  const size_t group_count = mins.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;

    for (size_t g = 0; g < group_count; ++g) {
      const uint32_t gmin = mins[g];
      const uint32_t gmax = maxs[g];

      if (gmax < low || gmin > high) continue;

      size_t start = static_cast<size_t>(g) * static_cast<size_t>(block_size);
      if (start >= n) break;
      size_t end_idx = start + static_cast<size_t>(block_size);
      if (end_idx > n) end_idx = n;

      const uint32_t* p = data + start;
      const uint32_t* end = data + end_idx;

      while (p + 4 <= end) {
        uint32_t v0 = p[0];
        uint32_t v1 = p[1];
        uint32_t v2 = p[2];
        uint32_t v3 = p[3];
        if (v0 >= low && v0 <= high) local += v0;
        if (v1 >= low && v1 <= high) local += v1;
        if (v2 >= low && v2 <= high) local += v2;
        if (v3 >= low && v3 <= high) local += v3;
        p += 4;
      }
      while (p < end) {
        uint32_t v = *p++;
        if (v >= low && v <= high) local += v;
      }
    }

    sum = local;
  }

  return sum;
}
