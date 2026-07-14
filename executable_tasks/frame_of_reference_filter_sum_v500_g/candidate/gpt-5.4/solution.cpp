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

  const uint16_t* p = deltas.data();
  uint64_t sum = 0;

  // If the range is invalid, no value can match.
  if (low > high) return 0;

  // Values are base + delta, where delta is uint16_t.
  // Convert the filter to an equivalent delta-domain filter when possible.
  const uint32_t max_delta = 65535u;

  if (high < base) {
    return 0;
  }

  uint32_t dlow32 = 0;
  if (low > base) {
    dlow32 = low - base;
    if (dlow32 > max_delta) return 0;
  }

  uint32_t dhigh32 = high - base;
  if (dhigh32 > max_delta) dhigh32 = max_delta;

  if (dlow32 > dhigh32) return 0;

  const uint16_t dlow = static_cast<uint16_t>(dlow32);
  const uint16_t dhigh = static_cast<uint16_t>(dhigh32);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t i = 0;
    const std::size_t unroll = n & ~std::size_t(7);
    for (; i < unroll; i += 8) {
      const uint16_t d0 = p[i + 0];
      const uint16_t d1 = p[i + 1];
      const uint16_t d2 = p[i + 2];
      const uint16_t d3 = p[i + 3];
      const uint16_t d4 = p[i + 4];
      const uint16_t d5 = p[i + 5];
      const uint16_t d6 = p[i + 6];
      const uint16_t d7 = p[i + 7];

      if (d0 >= dlow && d0 <= dhigh) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d0));
      if (d1 >= dlow && d1 <= dhigh) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d1));
      if (d2 >= dlow && d2 <= dhigh) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d2));
      if (d3 >= dlow && d3 <= dhigh) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d3));
      if (d4 >= dlow && d4 <= dhigh) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d4));
      if (d5 >= dlow && d5 <= dhigh) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d5));
      if (d6 >= dlow && d6 <= dhigh) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d6));
      if (d7 >= dlow && d7 <= dhigh) local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d7));
    }

    for (; i < n; ++i) {
      const uint16_t d = p[i];
      if (d >= dlow && d <= dhigh) {
        local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d));
      }
    }

    sum = local_sum;
  }

  return sum;
}
