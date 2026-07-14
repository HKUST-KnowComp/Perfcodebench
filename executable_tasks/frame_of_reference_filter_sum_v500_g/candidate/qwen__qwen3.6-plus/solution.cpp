#include "interface.h"
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  uint64_t sum = 0;
  const uint16_t* data = deltas.data();
  const size_t n = deltas.size();
  
  if (high < base) return 0;
  
  uint32_t min_d = (low > base) ? (low - base) : 0;
  uint32_t max_d = high - base;
  if (max_d > 65535) max_d = 65535;
  
  if (min_d > max_d) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t count = 0;
    uint64_t sum_d = 0;
    for (size_t i = 0; i < n; ++i) {
      uint16_t d = data[i];
      if (d >= min_d && d <= max_d) {
        sum_d += d;
        count++;
      }
    }
    sum = static_cast<uint64_t>(base) * count + sum_d;
  }
  return sum;
}