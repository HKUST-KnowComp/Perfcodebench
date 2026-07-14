#include "interface.h"

#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  uint64_t sum = 0;
  const std::size_t size = packed.size();
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    
    // Process 12 bytes at a time (8 values)
    std::size_t i = 0;
    const std::size_t limit = (size / 12u) * 12u;
    
    for (; i < limit; i += 12u) {
      const uint8_t* p = &packed[i];
      
      // Decode 8 values from 12 bytes
      const uint32_t v0 = static_cast<uint32_t>(p[0]) | ((static_cast<uint32_t>(p[1]) & 0x0fu) << 8u);
      const uint32_t v1 = (static_cast<uint32_t>(p[1]) >> 4u) | (static_cast<uint32_t>(p[2]) << 4u);
      const uint32_t v2 = static_cast<uint32_t>(p[3]) | ((static_cast<uint32_t>(p[4]) & 0x0fu) << 8u);
      const uint32_t v3 = (static_cast<uint32_t>(p[4]) >> 4u) | (static_cast<uint32_t>(p[5]) << 4u);
      const uint32_t v4 = static_cast<uint32_t>(p[6]) | ((static_cast<uint32_t>(p[7]) & 0x0fu) << 8u);
      const uint32_t v5 = (static_cast<uint32_t>(p[7]) >> 4u) | (static_cast<uint32_t>(p[8]) << 4u);
      const uint32_t v6 = static_cast<uint32_t>(p[9]) | ((static_cast<uint32_t>(p[10]) & 0x0fu) << 8u);
      const uint32_t v7 = (static_cast<uint32_t>(p[10]) >> 4u) | (static_cast<uint32_t>(p[11]) << 4u);
      
      // Branchless accumulation using conditional moves
      local_sum += (v0 > thresh) ? v0 : 0u;
      local_sum += (v1 > thresh) ? v1 : 0u;
      local_sum += (v2 > thresh) ? v2 : 0u;
      local_sum += (v3 > thresh) ? v3 : 0u;
      local_sum += (v4 > thresh) ? v4 : 0u;
      local_sum += (v5 > thresh) ? v5 : 0u;
      local_sum += (v6 > thresh) ? v6 : 0u;
      local_sum += (v7 > thresh) ? v7 : 0u;
    }
    
    // Handle remaining values (less than 8)
    for (; i + 2u < size; i += 3u) {
      const uint32_t a = static_cast<uint32_t>(packed[i]) | 
                        ((static_cast<uint32_t>(packed[i + 1u]) & 0x0fu) << 8u);
      const uint32_t b = (static_cast<uint32_t>(packed[i + 1u]) >> 4u) | 
                        (static_cast<uint32_t>(packed[i + 2u]) << 4u);
      
      local_sum += (a > thresh) ? a : 0u;
      local_sum += (b > thresh) ? b : 0u;
    }
    
    sum = local_sum;
  }
  
  return sum;
}