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

  const uint32_t n = static_cast<uint32_t>(values.size());
  if (n == 0) return 0;

  // Invalid range: exact result is empty.
  if (low > high) return 0;

  // Fast path when metadata is unusable.
  if (block_size == 0 || mins.empty() || maxs.empty()) {
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t s = 0;
      const uint32_t* p = values.data();
      const uint32_t* end = p + n;
      while (p != end) {
        const uint32_t v = *p++;
        if (v >= low && v <= high) s += static_cast<uint64_t>(v);
      }
      sum = s;
    }
    return sum;
  }

  const uint32_t block_count = static_cast<uint32_t>(mins.size() < maxs.size() ? mins.size() : maxs.size());
  if (block_count == 0) return 0;

  const uint32_t* vals = values.data();
  const uint32_t* minp = mins.data();
  const uint32_t* maxp = maxs.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;

    uint32_t start = 0;
    for (uint32_t b = 0; b < block_count && start < n; ++b) {
      const uint32_t bmin = minp[b];
      const uint32_t bmax = maxp[b];

      // Prune blocks with no overlap.
      if (bmax < low || bmin > high) {
        uint32_t next = start + block_size;
        start = next < n ? next : n;
        continue;
      }

      uint32_t end = start + block_size;
      if (end > n) end = n;

      // If the whole block is guaranteed in range, sum without per-value checks.
      if (bmin >= low && bmax <= high) {
        const uint32_t* p = vals + start;
        const uint32_t* e = vals + end;

        uint64_t local = 0;
        while (p + 4 <= e) {
          local += static_cast<uint64_t>(p[0]);
          local += static_cast<uint64_t>(p[1]);
          local += static_cast<uint64_t>(p[2]);
          local += static_cast<uint64_t>(p[3]);
          p += 4;
        }
        while (p != e) {
          local += static_cast<uint64_t>(*p++);
        }
        s += local;
      } else {
        const uint32_t* p = vals + start;
        const uint32_t* e = vals + end;
        uint64_t local = 0;
        while (p + 4 <= e) {
          const uint32_t v0 = p[0];
          const uint32_t v1 = p[1];
          const uint32_t v2 = p[2];
          const uint32_t v3 = p[3];
          if (v0 >= low && v0 <= high) local += static_cast<uint64_t>(v0);
          if (v1 >= low && v1 <= high) local += static_cast<uint64_t>(v1);
          if (v2 >= low && v2 <= high) local += static_cast<uint64_t>(v2);
          if (v3 >= low && v3 <= high) local += static_cast<uint64_t>(v3);
          p += 4;
        }
        while (p != e) {
          const uint32_t v = *p++;
          if (v >= low && v <= high) local += static_cast<uint64_t>(v);
        }
        s += local;
      }

      start = end;
    }

    // If metadata covers fewer blocks than the value tail, conservatively scan the rest.
    if (start < n) {
      const uint32_t* p = vals + start;
      const uint32_t* e = vals + n;
      while (p != e) {
        const uint32_t v = *p++;
        if (v >= low && v <= high) s += static_cast<uint64_t>(v);
      }
    }

    sum = s;
  }

  return sum;
}
