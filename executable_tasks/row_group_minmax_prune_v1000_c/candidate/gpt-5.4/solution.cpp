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
  if (low > high) return 0;

  const size_t num_groups = mins.size();
  if (block_size == 0 || num_groups == 0 || maxs.size() != num_groups) {
    uint64_t sum = 0;
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
  const uint32_t* mins_p = mins.data();
  const uint32_t* maxs_p = maxs.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;
    size_t base = 0;

    for (size_t g = 0; g < num_groups && base < n; ++g) {
      const uint32_t gmin = mins_p[g];
      const uint32_t gmax = maxs_p[g];

      if (gmax < low || gmin > high) {
        base += block_size;
        continue;
      }

      size_t end = base + block_size;
      if (end > n) end = n;
      const uint32_t* p = vals + base;
      const uint32_t* e = vals + end;

      if (gmin >= low && gmax <= high) {
        uint64_t block_sum = 0;
        while (p + 4 <= e) {
          block_sum += static_cast<uint64_t>(p[0]);
          block_sum += static_cast<uint64_t>(p[1]);
          block_sum += static_cast<uint64_t>(p[2]);
          block_sum += static_cast<uint64_t>(p[3]);
          p += 4;
        }
        while (p != e) {
          block_sum += static_cast<uint64_t>(*p++);
        }
        local += block_sum;
      } else {
        uint64_t block_sum = 0;
        while (p + 4 <= e) {
          const uint32_t v0 = p[0];
          const uint32_t v1 = p[1];
          const uint32_t v2 = p[2];
          const uint32_t v3 = p[3];
          if (v0 >= low && v0 <= high) block_sum += static_cast<uint64_t>(v0);
          if (v1 >= low && v1 <= high) block_sum += static_cast<uint64_t>(v1);
          if (v2 >= low && v2 <= high) block_sum += static_cast<uint64_t>(v2);
          if (v3 >= low && v3 <= high) block_sum += static_cast<uint64_t>(v3);
          p += 4;
        }
        while (p != e) {
          const uint32_t v = *p++;
          if (v >= low && v <= high) block_sum += static_cast<uint64_t>(v);
        }
        local += block_sum;
      }

      base = end;
    }

    if (base < n) {
      const uint32_t* p = vals + base;
      const uint32_t* e = vals + n;
      uint64_t tail_sum = 0;
      while (p + 4 <= e) {
        const uint32_t v0 = p[0];
        const uint32_t v1 = p[1];
        const uint32_t v2 = p[2];
        const uint32_t v3 = p[3];
        if (v0 >= low && v0 <= high) tail_sum += static_cast<uint64_t>(v0);
        if (v1 >= low && v1 <= high) tail_sum += static_cast<uint64_t>(v1);
        if (v2 >= low && v2 <= high) tail_sum += static_cast<uint64_t>(v2);
        if (v3 >= low && v3 <= high) tail_sum += static_cast<uint64_t>(v3);
        p += 4;
      }
      while (p != e) {
        const uint32_t v = *p++;
        if (v >= low && v <= high) tail_sum += static_cast<uint64_t>(v);
      }
      local += tail_sum;
    }

    sum = local;
  }

  return sum;
}
