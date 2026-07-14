#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    uint32_t value;
    for (std::size_t i = 0; i < deltas.size(); ++i) {
      value = base + static_cast<uint32_t>(deltas[i]);
      if (value >= low && value <= high) {
        iter_sum += static_cast<uint64_t>(value);
      }
    }
    sum = iter_sum;
  }
  return sum;
}