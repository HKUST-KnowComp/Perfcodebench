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
  const uint32_t* data = values.data();
  const size_t n = values.size();
  const size_t group_count = mins.size();

  if (iters <= 0 || n == 0) return 0;

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    if (block_size == 0 || group_count == 0) {
      const uint32_t* p = data;
      const uint32_t* end = data + n;
      while (p != end) {
        const uint32_t v = *p++;
        if (v >= low && v <= high) local_sum += static_cast<uint64_t>(v);
      }
      sum = local_sum;
      continue;
    }

    size_t idx = 0;
    for (size_t g = 0; g < group_count && idx < n; ++g) {
      const uint32_t gmin = mins[g];
      const uint32_t gmax = maxs[g];

      if (gmax < low || gmin > high) {
        size_t next = idx + static_cast<size_t>(block_size);
        idx = next < n ? next : n;
        continue;
      }

      size_t end_idx = idx + static_cast<size_t>(block_size);
      if (end_idx > n) end_idx = n;

      const uint32_t* p = data + idx;
      const uint32_t* end = data + end_idx;

      if (gmin >= low && gmax <= high) {
        uint64_t block_sum = 0;
        while ((end - p) >= 4) {
          block_sum += static_cast<uint64_t>(p[0]);
          block_sum += static_cast<uint64_t>(p[1]);
          block_sum += static_cast<uint64_t>(p[2]);
          block_sum += static_cast<uint64_t>(p[3]);
          p += 4;
        }
        while (p != end) {
          block_sum += static_cast<uint64_t>(*p++);
        }
        local_sum += block_sum;
      } else {
        while ((end - p) >= 4) {
          const uint32_t v0 = p[0];
          const uint32_t v1 = p[1];
          const uint32_t v2 = p[2];
          const uint32_t v3 = p[3];
          if (v0 >= low && v0 <= high) local_sum += static_cast<uint64_t>(v0);
          if (v1 >= low && v1 <= high) local_sum += static_cast<uint64_t>(v1);
          if (v2 >= low && v2 <= high) local_sum += static_cast<uint64_t>(v2);
          if (v3 >= low && v3 <= high) local_sum += static_cast<uint64_t>(v3);
          p += 4;
        }
        while (p != end) {
          const uint32_t v = *p++;
          if (v >= low && v <= high) local_sum += static_cast<uint64_t>(v);
        }
      }

      idx = end_idx;
    }

    if (idx < n) {
      const uint32_t* p = data + idx;
      const uint32_t* end = data + n;
      while ((end - p) >= 4) {
        const uint32_t v0 = p[0];
        const uint32_t v1 = p[1];
        const uint32_t v2 = p[2];
        const uint32_t v3 = p[3];
        if (v0 >= low && v0 <= high) local_sum += static_cast<uint64_t>(v0);
        if (v1 >= low && v1 <= high) local_sum += static_cast<uint64_t>(v1);
        if (v2 >= low && v2 <= high) local_sum += static_cast<uint64_t>(v2);
        if (v3 >= low && v3 <= high) local_sum += static_cast<uint64_t>(v3);
        p += 4;
      }
      while (p != end) {
        const uint32_t v = *p++;
        if (v >= low && v <= high) local_sum += static_cast<uint64_t>(v);
      }
    }

    sum = local_sum;
  }

  return sum;
}
