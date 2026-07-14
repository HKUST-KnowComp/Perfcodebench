#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n = deltas.size();
  const uint16_t* data = deltas.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    size_t i = 0;
    
    // Process in chunks to allow compiler optimization
    for (; i + 3 < n; i += 4) {
      uint32_t v0 = base + data[i];
      uint32_t v1 = base + data[i+1];
      uint32_t v2 = base + data[i+2];
      uint32_t v3 = base + data[i+3];

      if (v0 >= low && v0 <= high) iter_sum += v0;
      if (v1 >= low && v1 <= high) iter_sum += v1;
      if (v2 >= low && v2 <= high) iter_sum += v2;
      if (v3 >= low && v3 <= high) iter_sum += v3;
    }

    // Handle remaining elements
    for (; i < n; ++i) {
      uint32_t v = base + data[i];
      if (v >= low && v <= high) {
        iter_sum += v;
      }
    }
    total_sum = iter_sum;
  }
  return total_sum;
}