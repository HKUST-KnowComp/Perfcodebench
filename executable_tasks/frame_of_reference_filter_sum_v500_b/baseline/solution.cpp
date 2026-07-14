#include "interface.h"

#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  std::vector<uint32_t> decoded;
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    decoded.resize(deltas.size());
    for (std::size_t i = 0; i < deltas.size(); ++i) {
      decoded[i] = base + static_cast<uint32_t>(deltas[i]);
    }
    sum = 0;
    for (uint32_t value : decoded) {
      if (value >= low && value <= high) {
        sum += static_cast<uint64_t>(value);
      }
    }
  }
  return sum;
}
