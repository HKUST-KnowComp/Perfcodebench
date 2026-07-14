#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  if (deltas.empty() || high < low) return 0;
  
  const uint16_t* data = deltas.data();
  size_t n = deltas.size();
  uint32_t range = high - low;
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      uint32_t val = base + data[i];
      current_sum += ((val - low) <= range) ? val : 0;
    }
    sum = current_sum;
  }
  return sum;
}