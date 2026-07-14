#include "interface.h"

#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (uint16_t delta : deltas) {
      uint32_t value = base + static_cast<uint32_t>(delta);
      if (value >= low && value <= high) {
        sum += static_cast<uint64_t>(value);
      }
    }
  }
  return sum;
}