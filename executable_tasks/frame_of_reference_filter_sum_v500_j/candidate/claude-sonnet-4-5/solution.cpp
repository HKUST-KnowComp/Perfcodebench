#include "interface.h"

#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  uint64_t sum = 0;
  const std::size_t size = deltas.size();
  const uint16_t* data = deltas.data();
  
  // Precompute adjusted bounds to avoid repeated arithmetic
  const uint32_t low_delta = (low >= base) ? (low - base) : 0;
  const uint32_t high_delta = (high >= base) ? (high - base) : UINT32_MAX;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    // Fused decode and filter in single pass
    // Process in blocks of 4 for better ILP and potential auto-vectorization
    std::size_t i = 0;
    const std::size_t block_end = size - (size % 4);
    
    for (; i < block_end; i += 4) {
      uint16_t d0 = data[i];
      uint16_t d1 = data[i + 1];
      uint16_t d2 = data[i + 2];
      uint16_t d3 = data[i + 3];
      
      // Check deltas against adjusted bounds (avoids full decode when out of range)
      if (d0 >= low_delta && d0 <= high_delta) {
        uint32_t val = base + d0;
        if (val >= low && val <= high) sum += val;
      }
      if (d1 >= low_delta && d1 <= high_delta) {
        uint32_t val = base + d1;
        if (val >= low && val <= high) sum += val;
      }
      if (d2 >= low_delta && d2 <= high_delta) {
        uint32_t val = base + d2;
        if (val >= low && val <= high) sum += val;
      }
      if (d3 >= low_delta && d3 <= high_delta) {
        uint32_t val = base + d3;
        if (val >= low && val <= high) sum += val;
      }
    }
    
    // Handle remaining elements
    for (; i < size; ++i) {
      uint16_t d = data[i];
      if (d >= low_delta && d <= high_delta) {
        uint32_t val = base + d;
        if (val >= low && val <= high) {
          sum += val;
        }
      }
    }
  }
  
  return sum;
}