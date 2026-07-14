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
  (void)iters;
  if (high < base) return 0;

  uint32_t d_min = (low > base) ? (low - base) : 0;
  uint32_t d_max = high - base;
  if (d_max > 65535) d_max = 65535;

  uint64_t sum = 0;
  const uint16_t* data = deltas.data();
  std::size_t n = deltas.size();

  for (std::size_t i = 0; i < n; ++i) {
    uint16_t d = data[i];
    if (d >= d_min && d <= d_max) {
      sum += base + d;
    }
  }
  return sum;
}
