#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  uint64_t sum = 0;
  const uint32_t max_possible_value = base + UINT16_MAX;

  // Early exit if no values can ever fall into the query range
  if (max_possible_value < low || base > high) {
    for (int iter = 0; iter < iters; ++iter) {
      sum = 0;
    }
    return sum;
  }

  // Precompute valid delta range once to avoid repeated calculations
  const uint32_t min_d = (low <= base) ? 0 : (low - base);
  const uint32_t max_d = (high >= max_possible_value) ? UINT16_MAX : (high - base);

  // Process all iterations with zero heap allocations and tight inner loop
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (uint16_t d : deltas) {
      if (d >= min_d && d <= max_d) {
        sum += static_cast<uint64_t>(base + d);
      }
    }
  }
  return sum;
}