#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  if (iters <= 0) return 0;
  if (high < base) return 0;

  uint32_t min_d = (low > base) ? (low - base) : 0;
  uint32_t max_d = high - base;

  if (min_d > max_d) return 0;
  if (min_d > 0xFFFF) return 0;
  if (max_d > 0xFFFF) max_d = 0xFFFF;

  const uint16_t* data = deltas.data();
  size_t n = deltas.size();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      uint32_t d = data[i];
      if (d >= min_d && d <= max_d) {
        current_sum += base + d;
      }
    }
    sum = current_sum;
  }
  return sum;
}