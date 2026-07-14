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

  // Compute the filtered sum directly from the frame-of-reference encoded data,
  // avoiding materialization of a decoded buffer.
  //
  // value = base + delta, with delta in [0, 65535].
  // We transform the predicate:
  //   low <= base + delta <= high
  // into a delta-range check, clamped to uint16_t domain.

  uint32_t min_delta32 = 0;
  if (low > base) min_delta32 = low - base;

  uint32_t max_delta32;
  if (high < base) {
    max_delta32 = 0;
    // Empty range because all decoded values are >= base > high.
    return 0;
  } else {
    max_delta32 = high - base;
  }

  if (min_delta32 > 65535u) return 0;
  if (max_delta32 > 65535u) max_delta32 = 65535u;
  if (min_delta32 > max_delta32) return 0;

  const uint16_t min_delta = static_cast<uint16_t>(min_delta32);
  const uint16_t max_delta = static_cast<uint16_t>(max_delta32);

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    const uint16_t* p = deltas.data();
    const uint16_t* const end = p + n;

    for (; p != end; ++p) {
      const uint16_t d = *p;
      if (d >= min_delta && d <= max_delta) {
        local_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d));
      }
    }

    sum = local_sum;
  }

  return sum;
}
