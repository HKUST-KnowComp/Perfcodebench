#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  // Precompute valid delta range once, outside all inner loops
  const uint32_t delta_min = low >= base ? (low - base) : 0U;
  const uint32_t delta_max = (high <= base + UINT16_MAX) ? (high - base) : UINT16_MAX;

  // Fast path: no values can ever fall in the range
  if (delta_min > UINT16_MAX || delta_max < delta_min) {
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) sum = 0;
    return sum;
  }

  const uint16_t valid_min = static_cast<uint16_t>(delta_min);
  const uint16_t valid_max = static_cast<uint16_t>(delta_max);

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    // Process deltas directly, no intermediate decoded array needed
    for (const uint16_t d : deltas) {
      if (d >= valid_min && d <= valid_max) {
        sum += static_cast<uint64_t>(base) + static_cast<uint32_t>(d);
      }
    }
  }
  return sum;
}