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
  const uint16_t* __restrict__ data = deltas.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    // Process in chunks of 4 for better ILP and vectorization
    std::size_t i = 0;
    const std::size_t limit = size - (size % 4);
    
    for (; i < limit; i += 4) {
      uint32_t v0 = base + static_cast<uint32_t>(data[i]);
      uint32_t v1 = base + static_cast<uint32_t>(data[i + 1]);
      uint32_t v2 = base + static_cast<uint32_t>(data[i + 2]);
      uint32_t v3 = base + static_cast<uint32_t>(data[i + 3]);
      
      // Branchless filtering using conditional moves
      bool c0 = (v0 >= low) & (v0 <= high);
      bool c1 = (v1 >= low) & (v1 <= high);
      bool c2 = (v2 >= low) & (v2 <= high);
      bool c3 = (v3 >= low) & (v3 <= high);
      
      sum += c0 ? static_cast<uint64_t>(v0) : 0;
      sum += c1 ? static_cast<uint64_t>(v1) : 0;
      sum += c2 ? static_cast<uint64_t>(v2) : 0;
      sum += c3 ? static_cast<uint64_t>(v3) : 0;
    }
    
    // Handle remaining elements
    for (; i < size; ++i) {
      uint32_t value = base + static_cast<uint32_t>(data[i]);
      bool cond = (value >= low) & (value <= high);
      sum += cond ? static_cast<uint64_t>(value) : 0;
    }
  }
  
  return sum;
}
