#include "interface.h"

#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  // Preserve baseline behavior: if iters <= 0, no work is done, sum is 0.
  if (iters <= 0) return 0;

  // If range is invalid (low > high), baseline would scan and add nothing.
  if (low > high) return 0;

  const uint32_t b = base;
  const uint32_t lo = low;
  const uint32_t hi = high;

  const uint16_t* p = deltas.data();
  const size_t n = deltas.size();

  uint64_t s0 = 0, s1 = 0, s_tail = 0;

  size_t i = 0;
  // Unroll by 4 with two accumulators to improve ILP
  for (; i + 4 <= n; i += 4) {
    uint32_t v0 = static_cast<uint32_t>(b + static_cast<uint32_t>(p[i + 0]));
    if (v0 >= lo && v0 <= hi) s0 += static_cast<uint64_t>(v0);

    uint32_t v1 = static_cast<uint32_t>(b + static_cast<uint32_t>(p[i + 1]));
    if (v1 >= lo && v1 <= hi) s1 += static_cast<uint64_t>(v1);

    uint32_t v2 = static_cast<uint32_t>(b + static_cast<uint32_t>(p[i + 2]));
    if (v2 >= lo && v2 <= hi) s0 += static_cast<uint64_t>(v2);

    uint32_t v3 = static_cast<uint32_t>(b + static_cast<uint32_t>(p[i + 3]));
    if (v3 >= lo && v3 <= hi) s1 += static_cast<uint64_t>(v3);
  }

  // Handle remaining elements
  for (; i < n; ++i) {
    uint32_t v = static_cast<uint32_t>(b + static_cast<uint32_t>(p[i]));
    if (v >= lo && v <= hi) s_tail += static_cast<uint64_t>(v);
  }

  return s0 + s1 + s_tail;
}
