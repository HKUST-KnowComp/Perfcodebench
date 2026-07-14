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
  if (iters <= 0 || n == 0) return 0;

  // All decoded values are base + delta where delta is uint16_t.
  // Convert the filter range into an equivalent delta range once,
  // then sum matching decoded values directly without materializing
  // a decoded buffer.
  const uint32_t max_delta = 65535u;

  uint32_t min_d;
  if (low <= base) {
    min_d = 0u;
  } else {
    min_d = low - base;
    if (min_d > max_delta) return 0;
  }

  uint32_t max_d;
  if (high < base) {
    return 0;
  } else {
    max_d = high - base;
    if (max_d > max_delta) max_d = max_delta;
  }

  if (min_d > max_d) return 0;

  const uint16_t lo_d = static_cast<uint16_t>(min_d);
  const uint16_t hi_d = static_cast<uint16_t>(max_d);

  uint64_t sum = 0;
  const uint16_t* p = deltas.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;

    // Unroll to reduce loop overhead and help vectorization.
    for (; i + 8 <= n; i += 8) {
      const uint16_t d0 = p[i + 0];
      const uint16_t d1 = p[i + 1];
      const uint16_t d2 = p[i + 2];
      const uint16_t d3 = p[i + 3];
      const uint16_t d4 = p[i + 4];
      const uint16_t d5 = p[i + 5];
      const uint16_t d6 = p[i + 6];
      const uint16_t d7 = p[i + 7];

      if (d0 >= lo_d && d0 <= hi_d) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d0));
      if (d1 >= lo_d && d1 <= hi_d) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d1));
      if (d2 >= lo_d && d2 <= hi_d) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d2));
      if (d3 >= lo_d && d3 <= hi_d) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d3));
      if (d4 >= lo_d && d4 <= hi_d) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d4));
      if (d5 >= lo_d && d5 <= hi_d) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d5));
      if (d6 >= lo_d && d6 <= hi_d) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d6));
      if (d7 >= lo_d && d7 <= hi_d) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d7));
    }

    for (; i < n; ++i) {
      const uint16_t d = p[i];
      if (d >= lo_d && d <= hi_d) {
        local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d));
      }
    }

    sum = local_sum;
  }

  return sum;
}
