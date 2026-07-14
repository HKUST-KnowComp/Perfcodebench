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
  
  // Precompute filter bounds relative to base
  const uint32_t low_delta = (low > base) ? (low - base) : 0;
  const uint32_t high_delta = (high > base) ? (high - base) : UINT32_MAX;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    // Fused decode and filter in single pass
    for (std::size_t i = 0; i < size; ++i) {
      const uint32_t delta = data[i];
      
      // Check delta range first (cheaper than full value check)
      if (delta >= low_delta && delta <= high_delta) {
        const uint32_t value = base + delta;
        
        // Verify actual value is in range (handles edge cases)
        if (value >= low && value <= high) {
          sum += value;
        }
      }
    }
  }
  
  return sum;
}