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

  const size_t n = values.size();
  if (n == 0) return 0;

  // Preserve baseline semantics for invalid/empty ranges.
  if (low > high) return 0;

  const uint32_t* data = values.data();
  const uint32_t* mins_data = mins.empty() ? nullptr : mins.data();
  const uint32_t* maxs_data = maxs.empty() ? nullptr : maxs.data();

  uint64_t sum = 0;

  // Use row-group pruning only when metadata and block sizing are usable.
  const bool can_prune =
      block_size != 0 && mins_data != nullptr && maxs_data != nullptr;

  if (!can_prune) {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t local_sum = 0;
      const uint32_t* p = data;
      const uint32_t* end = data + n;

      while (p + 4 <= end) {
        uint32_t v0 = p[0];
        uint32_t v1 = p[1];
        uint32_t v2 = p[2];
        uint32_t v3 = p[3];
        if (v0 >= low && v0 <= high) local_sum += static_cast<uint64_t>(v0);
        if (v1 >= low && v1 <= high) local_sum += static_cast<uint64_t>(v1);
        if (v2 >= low && v2 <= high) local_sum += static_cast<uint64_t>(v2);
        if (v3 >= low && v3 <= high) local_sum += static_cast<uint64_t>(v3);
        p += 4;
      }
      while (p < end) {
        uint32_t v = *p++;
        if (v >= low && v <= high) local_sum += static_cast<uint64_t>(v);
      }
      sum = local_sum;
    }
    return sum;
  }

  const size_t num_blocks = mins.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    size_t base = 0;
    for (size_t b = 0; b < num_blocks && base < n; ++b, base += block_size) {
      const uint32_t bmin = mins_data[b];
      const uint32_t bmax = maxs_data[b];

      // No overlap with query range: prune block.
      if (bmax < low || bmin > high) continue;

      size_t end_idx = base + block_size;
      if (end_idx > n) end_idx = n;

      const uint32_t* p = data + base;
      const uint32_t* end = data + end_idx;

      // Full containment: sum whole block without per-value filtering.
      if (bmin >= low && bmax <= high) {
        while (p + 4 <= end) {
          local_sum += static_cast<uint64_t>(p[0]);
          local_sum += static_cast<uint64_t>(p[1]);
          local_sum += static_cast<uint64_t>(p[2]);
          local_sum += static_cast<uint64_t>(p[3]);
          p += 4;
        }
        while (p < end) {
          local_sum += static_cast<uint64_t>(*p++);
        }
      } else {
        while (p + 4 <= end) {
          uint32_t v0 = p[0];
          uint32_t v1 = p[1];
          uint32_t v2 = p[2];
          uint32_t v3 = p[3];
          if (v0 >= low && v0 <= high) local_sum += static_cast<uint64_t>(v0);
          if (v1 >= low && v1 <= high) local_sum += static_cast<uint64_t>(v1);
          if (v2 >= low && v2 <= high) local_sum += static_cast<uint64_t>(v2);
          if (v3 >= low && v3 <= high) local_sum += static_cast<uint64_t>(v3);
          p += 4;
        }
        while (p < end) {
          uint32_t v = *p++;
          if (v >= low && v <= high) local_sum += static_cast<uint64_t>(v);
        }
      }
    }

    // If metadata covered fewer blocks than needed, safely scan the tail.
    if (base < n) {
      const uint32_t* p = data + base;
      const uint32_t* end = data + n;
      while (p + 4 <= end) {
        uint32_t v0 = p[0];
        uint32_t v1 = p[1];
        uint32_t v2 = p[2];
        uint32_t v3 = p[3];
        if (v0 >= low && v0 <= high) local_sum += static_cast<uint64_t>(v0);
        if (v1 >= low && v1 <= high) local_sum += static_cast<uint64_t>(v1);
        if (v2 >= low && v2 <= high) local_sum += static_cast<uint64_t>(v2);
        if (v3 >= low && v3 <= high) local_sum += static_cast<uint64_t>(v3);
        p += 4;
      }
      while (p < end) {
        uint32_t v = *p++;
        if (v >= low && v <= high) local_sum += static_cast<uint64_t>(v);
      }
    }

    sum = local_sum;
  }

  return sum;
}
