#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  const size_t n = values.size();

  // Fallback to full scan if metadata cannot be used
  auto full_scan = [&](int iterations) -> uint64_t {
    uint64_t sum = 0;
    for (int iter = 0; iter < iterations; ++iter) {
      sum = 0;
      const uint32_t* p = values.data();
      const uint32_t* e = p + n;
      // Unroll by 4 for better throughput
      while (p + 4 <= e) {
        uint32_t v0 = p[0]; if (v0 >= low && v0 <= high) sum += static_cast<uint64_t>(v0);
        uint32_t v1 = p[1]; if (v1 >= low && v1 <= high) sum += static_cast<uint64_t>(v1);
        uint32_t v2 = p[2]; if (v2 >= low && v2 <= high) sum += static_cast<uint64_t>(v2);
        uint32_t v3 = p[3]; if (v3 >= low && v3 <= high) sum += static_cast<uint64_t>(v3);
        p += 4;
      }
      while (p < e) {
        uint32_t v = *p++;
        if (v >= low && v <= high) sum += static_cast<uint64_t>(v);
      }
    }
    return sum;
  };

  if (block_size == 0) {
    return full_scan(iters);
  }

  // Compute number of groups and validate metadata sizes
  const size_t groups = (n + static_cast<size_t>(block_size) - 1) / static_cast<size_t>(block_size);
  if (mins.size() < groups || maxs.size() < groups) {
    return full_scan(iters);
  }

  // Precompute group statuses: 0=skip, 1=full include, 2=partial
  std::vector<uint8_t> status;
  status.resize(groups);
  for (size_t g = 0; g < groups; ++g) {
    uint32_t gmin = mins[g];
    uint32_t gmax = maxs[g];
    if (gmax < low || gmin > high) {
      status[g] = 0; // completely outside
    } else if (gmin >= low && gmax <= high) {
      status[g] = 1; // completely inside
    } else {
      status[g] = 2; // partial overlap
    }
  }

  uint64_t sum = 0;
  const uint32_t* data = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t start = 0;
    for (size_t g = 0; g < groups; ++g) {
      size_t end = start + static_cast<size_t>(block_size);
      if (end > n) end = n;
      const uint8_t st = status[g];

      if (st == 1) {
        // Sum entire block
        const uint32_t* p = data + start;
        const uint32_t* e = data + end;
        while (p + 4 <= e) {
          sum += static_cast<uint64_t>(p[0]);
          sum += static_cast<uint64_t>(p[1]);
          sum += static_cast<uint64_t>(p[2]);
          sum += static_cast<uint64_t>(p[3]);
          p += 4;
        }
        while (p < e) {
          sum += static_cast<uint64_t>(*p++);
        }
      } else if (st == 2) {
        // Partial overlap: filter within block
        const uint32_t* p = data + start;
        const uint32_t* e = data + end;
        while (p + 4 <= e) {
          uint32_t v0 = p[0]; if (v0 >= low && v0 <= high) sum += static_cast<uint64_t>(v0);
          uint32_t v1 = p[1]; if (v1 >= low && v1 <= high) sum += static_cast<uint64_t>(v1);
          uint32_t v2 = p[2]; if (v2 >= low && v2 <= high) sum += static_cast<uint64_t>(v2);
          uint32_t v3 = p[3]; if (v3 >= low && v3 <= high) sum += static_cast<uint64_t>(v3);
          p += 4;
        }
        while (p < e) {
          uint32_t v = *p++;
          if (v >= low && v <= high) sum += static_cast<uint64_t>(v);
        }
      }

      start = end;
    }
  }

  return sum;
}
