#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  if (iters <= 0 || deltas.empty()) {
    return 0;
  }

  const std::size_t n = deltas.size();
  uint64_t sum = 0;

  // Fast reject when the filter range is entirely below the minimum possible
  // decoded value.
  if (high < base) {
    return 0;
  }

  // Compute the delta range that can satisfy:
  //   low <= base + delta <= high
  // using unsigned-safe logic.
  uint32_t min_delta32 = 0;
  if (low > base) {
    min_delta32 = low - base;
  }

  uint32_t max_delta32 = high - base;

  // Deltas are uint16_t, so clamp the admissible delta range accordingly.
  if (min_delta32 > 65535u) {
    return 0;
  }
  if (max_delta32 > 65535u) {
    max_delta32 = 65535u;
  }

  const uint16_t min_delta = static_cast<uint16_t>(min_delta32);
  const uint16_t max_delta = static_cast<uint16_t>(max_delta32);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    const uint16_t* p = deltas.data();
    std::size_t i = 0;

    // Unroll to reduce loop overhead.
    for (; i + 4 <= n; i += 4) {
      const uint16_t d0 = p[i];
      const uint16_t d1 = p[i + 1];
      const uint16_t d2 = p[i + 2];
      const uint16_t d3 = p[i + 3];

      if (d0 >= min_delta && d0 <= max_delta) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d0));
      if (d1 >= min_delta && d1 <= max_delta) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d1));
      if (d2 >= min_delta && d2 <= max_delta) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d2));
      if (d3 >= min_delta && d3 <= max_delta) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d3));
    }

    for (; i < n; ++i) {
      const uint16_t d = p[i];
      if (d >= min_delta && d <= max_delta) {
        local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d));
      }
    }

    sum = local_sum;
  }

  return sum;
}
