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

  // Decoded values are base + delta, where delta is uint16_t in [0, 65535].
  // If the requested range does not intersect this decoded domain, the sum is 0.
  const uint32_t decoded_min = base;
  const uint32_t decoded_max = base + 65535u;
  if (high < decoded_min || low > decoded_max || low > high) {
    return 0;
  }

  const uint16_t* p = deltas.data();
  const uint16_t* const end = p + n;
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    p = deltas.data();

    while (p != end) {
      const uint32_t value = base + static_cast<uint32_t>(*p++);
      if (value >= low && value <= high) {
        local_sum += static_cast<uint64_t>(value);
      }
    }

    sum = local_sum;
  }

  return sum;
}
