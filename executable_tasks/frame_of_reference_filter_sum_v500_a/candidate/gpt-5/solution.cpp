#include "interface.h"

#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  (void)iters; // Sum is identical each iteration; compute once

  if (high < low) {
    return 0;
  }

  const std::size_t n = deltas.size();
  const uint16_t* p = n ? deltas.data() : nullptr;
  const uint32_t lo = low;
  const uint32_t hi = high;
  const uint32_t range = hi - lo;

  uint64_t sum = 0;

  std::size_t i = 0;
  const std::size_t limit = n & ~static_cast<std::size_t>(7);
  for (; i < limit; i += 8) {
    uint32_t v0 = base + static_cast<uint32_t>(p[i + 0]);
    uint32_t v1 = base + static_cast<uint32_t>(p[i + 1]);
    uint32_t v2 = base + static_cast<uint32_t>(p[i + 2]);
    uint32_t v3 = base + static_cast<uint32_t>(p[i + 3]);
    uint32_t v4 = base + static_cast<uint32_t>(p[i + 4]);
    uint32_t v5 = base + static_cast<uint32_t>(p[i + 5]);
    uint32_t v6 = base + static_cast<uint32_t>(p[i + 6]);
    uint32_t v7 = base + static_cast<uint32_t>(p[i + 7]);

    sum += ((v0 - lo) <= range) ? static_cast<uint64_t>(v0) : 0ULL;
    sum += ((v1 - lo) <= range) ? static_cast<uint64_t>(v1) : 0ULL;
    sum += ((v2 - lo) <= range) ? static_cast<uint64_t>(v2) : 0ULL;
    sum += ((v3 - lo) <= range) ? static_cast<uint64_t>(v3) : 0ULL;
    sum += ((v4 - lo) <= range) ? static_cast<uint64_t>(v4) : 0ULL;
    sum += ((v5 - lo) <= range) ? static_cast<uint64_t>(v5) : 0ULL;
    sum += ((v6 - lo) <= range) ? static_cast<uint64_t>(v6) : 0ULL;
    sum += ((v7 - lo) <= range) ? static_cast<uint64_t>(v7) : 0ULL;
  }

  for (; i < n; ++i) {
    uint32_t v = base + static_cast<uint32_t>(p[i]);
    if ((v - lo) <= range) {
      sum += static_cast<uint64_t>(v);
    }
  }

  return sum;
}
