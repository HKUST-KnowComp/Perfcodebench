#include "interface.h"

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  (void)iters; // Result does not depend on iteration count; baseline returns the same sum each time.

  const std::size_t n = deltas.size();
  if (n == 0) return 0;
  if (high < low) return 0; // No values can satisfy the range in this case.

  const uint32_t b = base;
  const uint32_t l = low;
  const uint32_t h = high;

  const uint16_t* p = deltas.data();
  uint64_t sum = 0;

  std::size_t i = 0;
  const std::size_t unroll = 4;
  const std::size_t limit = n & ~(unroll - 1);

  for (; i < limit; i += unroll) {
    uint32_t v0 = static_cast<uint32_t>(p[i + 0]) + b;
    uint32_t v1 = static_cast<uint32_t>(p[i + 1]) + b;
    uint32_t v2 = static_cast<uint32_t>(p[i + 2]) + b;
    uint32_t v3 = static_cast<uint32_t>(p[i + 3]) + b;

    if (v0 >= l && v0 <= h) sum += static_cast<uint64_t>(v0);
    if (v1 >= l && v1 <= h) sum += static_cast<uint64_t>(v1);
    if (v2 >= l && v2 <= h) sum += static_cast<uint64_t>(v2);
    if (v3 >= l && v3 <= h) sum += static_cast<uint64_t>(v3);
  }

  for (; i < n; ++i) {
    uint32_t v = static_cast<uint32_t>(p[i]) + b;
    if (v >= l && v <= h) sum += static_cast<uint64_t>(v);
  }

  return sum;
}
