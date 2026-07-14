#include "interface.h"

#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const std::size_t size = packed.size();
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    const uint8_t* data = packed.data();
    
    // Process 12 bytes (8 values) at a time for better ILP
    std::size_t i = 0;
    const std::size_t limit = (size / 12) * 12;
    
    for (; i < limit; i += 12) {
      // Decode 8 values from 12 bytes
      const uint32_t v0 = static_cast<uint32_t>(data[i]) | 
                          ((static_cast<uint32_t>(data[i + 1]) & 0x0fu) << 8u);
      const uint32_t v1 = (static_cast<uint32_t>(data[i + 1]) >> 4u) | 
                          (static_cast<uint32_t>(data[i + 2]) << 4u);
      const uint32_t v2 = static_cast<uint32_t>(data[i + 3]) | 
                          ((static_cast<uint32_t>(data[i + 4]) & 0x0fu) << 8u);
      const uint32_t v3 = (static_cast<uint32_t>(data[i + 4]) >> 4u) | 
                          (static_cast<uint32_t>(data[i + 5]) << 4u);
      const uint32_t v4 = static_cast<uint32_t>(data[i + 6]) | 
                          ((static_cast<uint32_t>(data[i + 7]) & 0x0fu) << 8u);
      const uint32_t v5 = (static_cast<uint32_t>(data[i + 7]) >> 4u) | 
                          (static_cast<uint32_t>(data[i + 8]) << 4u);
      const uint32_t v6 = static_cast<uint32_t>(data[i + 9]) | 
                          ((static_cast<uint32_t>(data[i + 10]) & 0x0fu) << 8u);
      const uint32_t v7 = (static_cast<uint32_t>(data[i + 10]) >> 4u) | 
                          (static_cast<uint32_t>(data[i + 11]) << 4u);
      
      // Branchless conditional accumulation
      local_sum += (v0 > thresh) ? v0 : 0;
      local_sum += (v1 > thresh) ? v1 : 0;
      local_sum += (v2 > thresh) ? v2 : 0;
      local_sum += (v3 > thresh) ? v3 : 0;
      local_sum += (v4 > thresh) ? v4 : 0;
      local_sum += (v5 > thresh) ? v5 : 0;
      local_sum += (v6 > thresh) ? v6 : 0;
      local_sum += (v7 > thresh) ? v7 : 0;
    }
    
    // Handle remaining bytes (0-11 bytes, 0-7 values)
    for (; i + 2 < size; i += 3) {
      const uint32_t v0 = static_cast<uint32_t>(data[i]) | 
                          ((static_cast<uint32_t>(data[i + 1]) & 0x0fu) << 8u);
      const uint32_t v1 = (static_cast<uint32_t>(data[i + 1]) >> 4u) | 
                          (static_cast<uint32_t>(data[i + 2]) << 4u);
      
      local_sum += (v0 > thresh) ? v0 : 0;
      local_sum += (v1 > thresh) ? v1 : 0;
    }
    
    sum = local_sum;
  }
  
  return sum;
}