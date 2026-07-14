#include "interface.h"
#include <cstdint>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  if (iters <= 0) return 0;
  uint64_t sum = 0;
  for (std::size_t i = 0; i < deltas.size(); ++i) {
    uint32_t val = base + static_cast<uint32_t>(deltas[i]);
    if (val >= low && val <= high) {
      sum += val;
    }
  }
  return sum;
}