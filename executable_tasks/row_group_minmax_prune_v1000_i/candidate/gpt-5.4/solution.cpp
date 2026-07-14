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
  const uint32_t* data = values.empty() ? nullptr : values.data();
  const uint32_t* minp = mins.empty() ? nullptr : mins.data();
  const uint32_t* maxp = maxs.empty() ? nullptr : maxs.data();
  const size_t n = values.size();

  if (iters <= 0 || n == 0) return 0;
  if (low > high) return 0;

  // Fallback to full scan if metadata is unusable.
  if (block_size == 0) {
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t s = 0;
      const uint32_t* p = data;
      const uint32_t* end = data + n;
      while (p != end) {
        uint32_t v = *p++;
        if (v >= low && v <= high) s += static_cast<uint64_t>(v);
      }
      sum = s;
    }
    return sum;
  }

  const size_t block_count = (n + static_cast<size_t>(block_size) - 1) / static_cast<size_t>(block_size);
  if (mins.size() < block_count || maxs.size() < block_count || minp == nullptr || maxp == nullptr) {
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t s = 0;
      const uint32_t* p = data;
      const uint32_t* end = data + n;
      while (p != end) {
        uint32_t v = *p++;
        if (v >= low && v <= high) s += static_cast<uint64_t>(v);
      }
      sum = s;
    }
    return sum;
  }

  uint64_t sum = 0;
  const size_t bs = static_cast<size_t>(block_size);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    size_t base = 0;

    for (size_t g = 0; g < block_count; ++g, base += bs) {
      const uint32_t gmin = minp[g];
      const uint32_t gmax = maxp[g];

      if (gmax < low || gmin > high) {
        continue;
      }

      size_t len = n - base;
      if (len > bs) len = bs;
      const uint32_t* p = data + base;
      const uint32_t* end = p + len;

      if (gmin >= low && gmax <= high) {
        uint64_t local = 0;
        while ((end - p) >= 8) {
          local += static_cast<uint64_t>(p[0]);
          local += static_cast<uint64_t>(p[1]);
          local += static_cast<uint64_t>(p[2]);
          local += static_cast<uint64_t>(p[3]);
          local += static_cast<uint64_t>(p[4]);
          local += static_cast<uint64_t>(p[5]);
          local += static_cast<uint64_t>(p[6]);
          local += static_cast<uint64_t>(p[7]);
          p += 8;
        }
        while (p != end) {
          local += static_cast<uint64_t>(*p++);
        }
        s += local;
      } else {
        uint64_t local = 0;
        while ((end - p) >= 8) {
          uint32_t v0 = p[0];
          uint32_t v1 = p[1];
          uint32_t v2 = p[2];
          uint32_t v3 = p[3];
          uint32_t v4 = p[4];
          uint32_t v5 = p[5];
          uint32_t v6 = p[6];
          uint32_t v7 = p[7];
          if (v0 >= low && v0 <= high) local += static_cast<uint64_t>(v0);
          if (v1 >= low && v1 <= high) local += static_cast<uint64_t>(v1);
          if (v2 >= low && v2 <= high) local += static_cast<uint64_t>(v2);
          if (v3 >= low && v3 <= high) local += static_cast<uint64_t>(v3);
          if (v4 >= low && v4 <= high) local += static_cast<uint64_t>(v4);
          if (v5 >= low && v5 <= high) local += static_cast<uint64_t>(v5);
          if (v6 >= low && v6 <= high) local += static_cast<uint64_t>(v6);
          if (v7 >= low && v7 <= high) local += static_cast<uint64_t>(v7);
          p += 8;
        }
        while (p != end) {
          uint32_t v = *p++;
          if (v >= low && v <= high) local += static_cast<uint64_t>(v);
        }
        s += local;
      }
    }

    sum = s;
  }

  return sum;
}
