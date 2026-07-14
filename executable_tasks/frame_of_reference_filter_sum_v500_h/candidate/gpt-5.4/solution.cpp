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

  // Decode and filter in a single pass; no temporary decoded buffer needed.
  // Since value = base + delta and delta is uint16_t, compute the admissible
  // delta range once and compare deltas directly.
  uint64_t sum = 0;

  const uint64_t base64 = static_cast<uint64_t>(base);
  const uint64_t dmin64 = (low > base) ? static_cast<uint64_t>(low - base) : 0ull;
  const uint64_t dmax64 = (high >= base) ? static_cast<uint64_t>(high - base) : static_cast<uint64_t>(-1);

  if (dmax64 > 65535ull) {
    if (dmin64 > 65535ull) {
      // No decoded value can reach the filter range.
      return 0;
    }

    const uint16_t dmin = static_cast<uint16_t>(dmin64);
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t local_sum = 0;
      const uint16_t* p = deltas.data();
      const uint16_t* end = p + n;
      for (; p != end; ++p) {
        const uint16_t d = *p;
        if (d >= dmin) {
          local_sum += base64 + static_cast<uint64_t>(d);
        }
      }
      sum = local_sum;
    }
    return sum;
  }

  const uint16_t dmin = static_cast<uint16_t>(dmin64);
  const uint16_t dmax = static_cast<uint16_t>(dmax64);

  if (dmin > dmax) {
    return 0;
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    const uint16_t* p = deltas.data();
    const uint16_t* end = p + n;

    for (; p != end; ++p) {
      const uint16_t d = *p;
      if (d >= dmin && d <= dmax) {
        local_sum += base64 + static_cast<uint64_t>(d);
      }
    }
    sum = local_sum;
  }

  return sum;
}
