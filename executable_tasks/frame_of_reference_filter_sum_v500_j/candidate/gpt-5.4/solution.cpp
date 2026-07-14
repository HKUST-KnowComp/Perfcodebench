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

  // If the filter range is invalid, nothing can match.
  if (low > high) return 0;

  // Compute the delta range that can satisfy:
  //   low <= base + delta <= high
  // using 64-bit arithmetic to avoid overflow/underflow.
  const int64_t min_delta64 = static_cast<int64_t>(low) - static_cast<int64_t>(base);
  const int64_t max_delta64 = static_cast<int64_t>(high) - static_cast<int64_t>(base);

  if (max_delta64 < 0 || min_delta64 > 65535) {
    return 0;
  }

  uint32_t min_delta = min_delta64 <= 0 ? 0u : static_cast<uint32_t>(min_delta64);
  uint32_t max_delta = max_delta64 >= 65535 ? 65535u : static_cast<uint32_t>(max_delta64);

  if (min_delta > max_delta) return 0;

  const uint16_t min_d = static_cast<uint16_t>(min_delta);
  const uint16_t max_d = static_cast<uint16_t>(max_delta);

  uint64_t one_pass_sum = 0;
  const uint16_t* p = deltas.data();
  const uint16_t* end = p + n;

  for (; p != end; ++p) {
    const uint16_t d = *p;
    if (d >= min_d && d <= max_d) {
      one_pass_sum += static_cast<uint64_t>(base + static_cast<uint32_t>(d));
    }
  }

  return one_pass_sum * static_cast<uint64_t>(iters);
}
