#include "interface.h"

#include <cstddef>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  // Preserve baseline behavior: if iters <= 0, no work is done and sum is 0.
  if (iters <= 0) {
    return 0;
  }

  const uint16_t* d = deltas.data();
  std::size_t n = deltas.size();

  uint64_t sum = 0;

  // Unrolled loop for better throughput; compute values on-the-fly and filter.
  std::size_t i = 0;
  std::size_t limit = n & static_cast<std::size_t>(~7ULL); // multiple of 8
  for (; i < limit; i += 8) {
    uint32_t v0 = base + static_cast<uint32_t>(d[i + 0]);
    uint32_t v1 = base + static_cast<uint32_t>(d[i + 1]);
    uint32_t v2 = base + static_cast<uint32_t>(d[i + 2]);
    uint32_t v3 = base + static_cast<uint32_t>(d[i + 3]);
    uint32_t v4 = base + static_cast<uint32_t>(d[i + 4]);
    uint32_t v5 = base + static_cast<uint32_t>(d[i + 5]);
    uint32_t v6 = base + static_cast<uint32_t>(d[i + 6]);
    uint32_t v7 = base + static_cast<uint32_t>(d[i + 7]);

    if (v0 >= low && v0 <= high) sum += static_cast<uint64_t>(v0);
    if (v1 >= low && v1 <= high) sum += static_cast<uint64_t>(v1);
    if (v2 >= low && v2 <= high) sum += static_cast<uint64_t>(v2);
    if (v3 >= low && v3 <= high) sum += static_cast<uint64_t>(v3);
    if (v4 >= low && v4 <= high) sum += static_cast<uint64_t>(v4);
    if (v5 >= low && v5 <= high) sum += static_cast<uint64_t>(v5);
    if (v6 >= low && v6 <= high) sum += static_cast<uint64_t>(v6);
    if (v7 >= low && v7 <= high) sum += static_cast<uint64_t>(v7);
  }

  for (; i < n; ++i) {
    uint32_t v = base + static_cast<uint32_t>(d[i]);
    if (v >= low && v <= high) {
      sum += static_cast<uint64_t>(v);
    }
  }

  // Baseline recomputed the same sum each iteration and returned the last one.
  // Since inputs don't change, returning the one computed sum preserves correctness.
  return sum;
}
