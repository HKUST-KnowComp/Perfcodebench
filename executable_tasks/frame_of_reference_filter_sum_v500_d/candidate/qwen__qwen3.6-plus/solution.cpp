#include "interface.h"
#include <vector>
#include <cstddef>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  const uint16_t* data = deltas.data();
  const size_t n = deltas.size();
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    for (size_t i = 0; i < n; ++i) {
      uint32_t v = base + data[i];
      if (v >= low && v <= high) {
        s += v;
      }
    }
    sum = s;
  }
  return sum;
}