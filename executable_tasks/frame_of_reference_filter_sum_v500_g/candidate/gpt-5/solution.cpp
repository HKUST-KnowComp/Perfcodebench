#include "interface.h"

#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  // Preserve baseline semantics: if no iterations, return 0.
  if (iters <= 0) {
    return 0;
  }
  // If range is invalid, no values qualify (matches baseline behavior).
  if (low > high) {
    return 0;
  }

  const uint16_t* data = deltas.data();
  const std::size_t n = deltas.size();
  const uint32_t range = high - low;

  uint64_t sum = 0;

  std::size_t i = 0;
  // Unroll by 8 for better ILP and reduced loop overhead.
  for (; i + 8 <= n; i += 8) {
    uint32_t v0 = base + static_cast<uint32_t>(data[i + 0]);
    uint32_t v1 = base + static_cast<uint32_t>(data[i + 1]);
    uint32_t v2 = base + static_cast<uint32_t>(data[i + 2]);
    uint32_t v3 = base + static_cast<uint32_t>(data[i + 3]);
    uint32_t v4 = base + static_cast<uint32_t>(data[i + 4]);
    uint32_t v5 = base + static_cast<uint32_t>(data[i + 5]);
    uint32_t v6 = base + static_cast<uint32_t>(data[i + 6]);
    uint32_t v7 = base + static_cast<uint32_t>(data[i + 7]);

    uint64_t c0 = (static_cast<uint32_t>(v0 - low) <= range);
    uint64_t c1 = (static_cast<uint32_t>(v1 - low) <= range);
    uint64_t c2 = (static_cast<uint32_t>(v2 - low) <= range);
    uint64_t c3 = (static_cast<uint32_t>(v3 - low) <= range);
    uint64_t c4 = (static_cast<uint32_t>(v4 - low) <= range);
    uint64_t c5 = (static_cast<uint32_t>(v5 - low) <= range);
    uint64_t c6 = (static_cast<uint32_t>(v6 - low) <= range);
    uint64_t c7 = (static_cast<uint32_t>(v7 - low) <= range);

    sum += static_cast<uint64_t>(v0) * c0
         + static_cast<uint64_t>(v1) * c1
         + static_cast<uint64_t>(v2) * c2
         + static_cast<uint64_t>(v3) * c3
         + static_cast<uint64_t>(v4) * c4
         + static_cast<uint64_t>(v5) * c5
         + static_cast<uint64_t>(v6) * c6
         + static_cast<uint64_t>(v7) * c7;
  }

  // Handle remaining elements
  for (; i < n; ++i) {
    uint32_t v = base + static_cast<uint32_t>(data[i]);
    uint64_t c = (static_cast<uint32_t>(v - low) <= range);
    sum += static_cast<uint64_t>(v) * c;
  }

  // Baseline returns the last iteration's sum, which equals this sum.
  return sum;
}
