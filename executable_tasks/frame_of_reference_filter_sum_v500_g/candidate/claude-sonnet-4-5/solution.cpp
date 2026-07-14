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
  const uint32_t adjusted_low = (low >= base) ? (low - base) : 0;
  const uint32_t adjusted_high = (high >= base) ? (high - base) : UINT32_MAX;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    // Process in chunks of 4 for better instruction-level parallelism
    std::size_t i = 0;
    const std::size_t limit = size - (size % 4);
    
    for (; i < limit; i += 4) {
      uint32_t v0 = data[i];
      uint32_t v1 = data[i + 1];
      uint32_t v2 = data[i + 2];
      uint32_t v3 = data[i + 3];
      
      // Check against adjusted bounds (delta range)
      if (v0 >= adjusted_low && v0 <= adjusted_high) {
        sum += base + v0;
      }
      if (v1 >= adjusted_low && v1 <= adjusted_high) {
        sum += base + v1;
      }
      if (v2 >= adjusted_low && v2 <= adjusted_high) {
        sum += base + v2;
      }
      if (v3 >= adjusted_low && v3 <= adjusted_high) {
        sum += base + v3;
      }
    }
    
    // Handle remaining elements
    for (; i < size; ++i) {
      uint32_t delta = data[i];
      if (delta >= adjusted_low && delta <= adjusted_high) {
        sum += base + delta;
      }
    }
  }
  
  return sum;
}