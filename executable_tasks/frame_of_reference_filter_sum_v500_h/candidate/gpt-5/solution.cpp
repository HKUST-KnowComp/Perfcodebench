#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  // Match baseline behavior: if no iterations, return 0.
  if (iters <= 0) {
    return 0;
  }

  // If range is invalid, no values qualify.
  if (high < low) {
    return 0;
  }

  const std::size_t n = deltas.size();
  if (n == 0) {
    return 0;
  }

  // Compute valid delta range [minD, maxD] such that low <= base + d <= high.
  // Handle cases to avoid underflow/overflow.
  // If high < base, then even the smallest delta (0) makes base+d > high.
  if (high < base) {
    return 0;
  }

  uint32_t minD;
  if (low <= base) {
    minD = 0u;
  } else {
    uint32_t diff = low - base;
    if (diff > 0xFFFFu) {
      // Required minimum delta larger than possible uint16_t.
      return 0;
    }
    minD = diff;
  }

  uint32_t maxD;
  {
    uint32_t diff = high - base; // safe since high >= base here
    maxD = (diff > 0xFFFFu) ? 0xFFFFu : diff;
  }

  if (minD > maxD) {
    return 0;
  }

  const uint16_t minD16 = static_cast<uint16_t>(minD);
  const uint16_t maxD16 = static_cast<uint16_t>(maxD);

  // Accumulate count and sum of deltas within [minD16, maxD16].
  uint64_t count = 0;
  uint64_t sum_deltas = 0;

  const uint16_t* p = deltas.data();
  for (std::size_t i = 0; i < n; ++i) {
    uint16_t d = p[i];
    if (d >= minD16 && d <= maxD16) {
      ++count;
      sum_deltas += static_cast<uint64_t>(d);
    }
  }

  uint64_t sum = static_cast<uint64_t>(base) * count + sum_deltas;

  // Baseline returned the sum from the last iteration, which is identical each time.
  // So we just return the computed sum.
  return sum;
}
