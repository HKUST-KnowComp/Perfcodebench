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
  
  // Precompute adjusted bounds to avoid repeated arithmetic in hot loop
  const uint32_t adjusted_low = (low >= base) ? (low - base) : 0;
  const uint32_t adjusted_high = (high >= base) ? (high - base) : 0;
  const bool base_in_range = (base >= low && base <= high);
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    // Fused decode + filter in single pass
    // Process in chunks of 4 for better ILP and potential auto-vectorization
    std::size_t i = 0;
    const std::size_t limit = size - (size % 4);
    
    for (; i < limit; i += 4) {
      uint32_t d0 = data[i];
      uint32_t d1 = data[i + 1];
      uint32_t d2 = data[i + 2];
      uint32_t d3 = data[i + 3];
      
      // Check against adjusted bounds (delta space)
      if (base_in_range) {
        if (d0 >= adjusted_low && d0 <= adjusted_high) sum += base + d0;
        if (d1 >= adjusted_low && d1 <= adjusted_high) sum += base + d1;
        if (d2 >= adjusted_low && d2 <= adjusted_high) sum += base + d2;
        if (d3 >= adjusted_low && d3 <= adjusted_high) sum += base + d3;
      }
    }
    
    // Handle remainder
    for (; i < size; ++i) {
      uint32_t delta = data[i];
      if (base_in_range && delta >= adjusted_low && delta <= adjusted_high) {
        sum += base + delta;
      }
    }
  }
  
  return sum;
}