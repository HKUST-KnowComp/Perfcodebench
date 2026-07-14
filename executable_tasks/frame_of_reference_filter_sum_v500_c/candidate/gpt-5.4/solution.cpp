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

  // If the range is invalid, no value can match.
  if (low > high) return 0;

  // Decode-on-the-fly: value = base + delta, where delta is uint16_t.
  // Since delta is bounded, we can transform the filter into a delta-range
  // check and avoid per-element 32-bit addition in the common path.
  //
  // We need: low <= base + d <= high
  // <=> low - base <= d <= high - base, with care for underflow/overflow.
  uint32_t dlow32 = 0;
  if (low > base) dlow32 = low - base;

  uint32_t dhigh32;
  if (high < base) {
    return 0;
  } else {
    dhigh32 = high - base;
  }

  if (dlow32 > 65535u) return 0;
  if (dhigh32 > 65535u) dhigh32 = 65535u;
  if (dlow32 > dhigh32) return 0;

  const uint16_t dlow = static_cast<uint16_t>(dlow32);
  const uint16_t dhigh = static_cast<uint16_t>(dhigh32);

  const uint16_t* p = deltas.data();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;

    // Unroll to reduce loop overhead and help vectorization.
    for (; i + 7 < n; i += 8) {
      const uint16_t d0 = p[i + 0];
      const uint16_t d1 = p[i + 1];
      const uint16_t d2 = p[i + 2];
      const uint16_t d3 = p[i + 3];
      const uint16_t d4 = p[i + 4];
      const uint16_t d5 = p[i + 5];
      const uint16_t d6 = p[i + 6];
      const uint16_t d7 = p[i + 7];

      if (d0 >= dlow && d0 <= dhigh) local_sum += static_cast<uint64_t>(base + d0);
      if (d1 >= dlow && d1 <= dhigh) local_sum += static_cast<uint64_t>(base + d1);
      if (d2 >= dlow && d2 <= dhigh) local_sum += static_cast<uint64_t>(base + d2);
      if (d3 >= dlow && d3 <= dhigh) local_sum += static_cast<uint64_t>(base + d3);
      if (d4 >= dlow && d4 <= dhigh) local_sum += static_cast<uint64_t>(base + d4);
      if (d5 >= dlow && d5 <= dhigh) local_sum += static_cast<uint64_t>(base + d5);
      if (d6 >= dlow && d6 <= dhigh) local_sum += static_cast<uint64_t>(base + d6);
      if (d7 >= dlow && d7 <= dhigh) local_sum += static_cast<uint64_t>(base + d7);
    }

    for (; i < n; ++i) {
      const uint16_t d = p[i];
      if (d >= dlow && d <= dhigh) {
        local_sum += static_cast<uint64_t>(base + d);
      }
    }

    sum = local_sum;
  }

  return sum;
}
