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

  // Compute the valid delta range once, avoiding full decode and extra storage.
  // A value matches iff: low <= base + delta <= high.
  // Since delta is uint16_t, clamp the admissible delta interval to [0, 65535].
  uint32_t min_delta = 0;
  if (low > base) min_delta = low - base;

  uint32_t max_delta;
  if (high < base) {
    max_delta = 0;
    if (min_delta > 0) {
      // Empty range.
      return 0;
    }
  } else {
    max_delta = high - base;
  }

  if (min_delta > 65535u) return 0;
  if (max_delta > 65535u) max_delta = 65535u;
  if (min_delta > max_delta) return 0;

  const uint16_t lo_d = static_cast<uint16_t>(min_delta);
  const uint16_t hi_d = static_cast<uint16_t>(max_delta);

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    const uint16_t* p = deltas.data();
    const uint16_t* const end = p + n;

    while (p != end) {
      const uint16_t d = *p++;
      if (d >= lo_d && d <= hi_d) {
        local_sum += static_cast<uint64_t>(base) + static_cast<uint64_t>(d);
      }
    }

    sum = local_sum;
  }

  return sum;
}
