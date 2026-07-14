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
    std::size_t i = 0;
    
    // Process 4 elements at a time for better ILP
    const std::size_t simd_end = size - (size % 4);
    for (; i < simd_end; i += 4) {
      uint32_t d0 = data[i];
      uint32_t d1 = data[i + 1];
      uint32_t d2 = data[i + 2];
      uint32_t d3 = data[i + 3];
      
      // Check deltas against adjusted bounds
      if (d0 >= low_delta && d0 <= high_delta) {
        sum += base + d0;
      }
      if (d1 >= low_delta && d1 <= high_delta) {
        sum += base + d1;
      }
      if (d2 >= low_delta && d2 <= high_delta) {
        sum += base + d2;
      }
      if (d3 >= low_delta && d3 <= high_delta) {
        sum += base + d3;
      }
    }
    
    // Handle remaining elements
    for (; i < size; ++i) {
      uint32_t delta = data[i];
      if (delta >= low_delta && delta <= high_delta) {
        sum += base + delta;
      }
    }
  }
  
  return sum;
}