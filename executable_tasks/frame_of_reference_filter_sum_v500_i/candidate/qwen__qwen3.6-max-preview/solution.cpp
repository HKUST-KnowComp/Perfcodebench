#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  if (iters <= 0) return 0;
  
  uint64_t sum = 0;
  const uint16_t* d = deltas.data();
  std::size_t n = deltas.size();
  
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t val = base + static_cast<uint32_t>(d[i]);
    if (val >= low && val <= high) {
      sum += val;
    }
  }
  return sum;
}