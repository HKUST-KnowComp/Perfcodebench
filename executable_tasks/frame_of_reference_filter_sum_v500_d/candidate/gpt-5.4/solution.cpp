#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  const std::size_t n = deltas.size();
  if (iters <= 0 || n == 0) {
    return 0;
  }

  const uint16_t* p = deltas.data();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t i = 0;
    const std::size_t unroll = 8;
    const std::size_t limit = n & ~(unroll - 1);

    for (; i < limit; i += unroll) {
      uint32_t v0 = base + static_cast<uint32_t>(p[i + 0]);
      uint32_t v1 = base + static_cast<uint32_t>(p[i + 1]);
      uint32_t v2 = base + static_cast<uint32_t>(p[i + 2]);
      uint32_t v3 = base + static_cast<uint32_t>(p[i + 3]);
      uint32_t v4 = base + static_cast<uint32_t>(p[i + 4]);
      uint32_t v5 = base + static_cast<uint32_t>(p[i + 5]);
      uint32_t v6 = base + static_cast<uint32_t>(p[i + 6]);
      uint32_t v7 = base + static_cast<uint32_t>(p[i + 7]);

      if (v0 >= low && v0 <= high) local_sum += static_cast<uint64_t>(v0);
      if (v1 >= low && v1 <= high) local_sum += static_cast<uint64_t>(v1);
      if (v2 >= low && v2 <= high) local_sum += static_cast<uint64_t>(v2);
      if (v3 >= low && v3 <= high) local_sum += static_cast<uint64_t>(v3);
      if (v4 >= low && v4 <= high) local_sum += static_cast<uint64_t>(v4);
      if (v5 >= low && v5 <= high) local_sum += static_cast<uint64_t>(v5);
      if (v6 >= low && v6 <= high) local_sum += static_cast<uint64_t>(v6);
      if (v7 >= low && v7 <= high) local_sum += static_cast<uint64_t>(v7);
    }

    for (; i < n; ++i) {
      uint32_t v = base + static_cast<uint32_t>(p[i]);
      if (v >= low && v <= high) {
        local_sum += static_cast<uint64_t>(v);
      }
    }

    sum = local_sum;
  }

  return sum;
}
