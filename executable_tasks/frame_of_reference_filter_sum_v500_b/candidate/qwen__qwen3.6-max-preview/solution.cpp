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
  uint64_t sum = 0;
  const uint16_t* data = deltas.data();
  const size_t n = deltas.size();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (size_t i = 0; i < n; ++i) {
      uint32_t v = base + data[i];
      if (v >= low && v <= high) {
        sum += v;
      }
    }
  }
  return sum;
}