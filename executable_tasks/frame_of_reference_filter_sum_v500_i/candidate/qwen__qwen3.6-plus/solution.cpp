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
  const std::size_t n = deltas.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t val = base + data[i];
      if (val >= low && val <= high) {
        current_sum += val;
      }
    }
    sum = current_sum;
  }
  return sum;
}