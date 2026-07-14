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
    const std::size_t limit = (size / 12u) * 12u;
    
    for (; i < limit; i += 12u) {
      // First group of 3 bytes -> 2 values
      uint32_t a0 = static_cast<uint32_t>(data[i]) | 
                    ((static_cast<uint32_t>(data[i + 1u]) & 0x0fu) << 8u);
      uint32_t b0 = (static_cast<uint32_t>(data[i + 1u]) >> 4u) | 
                    (static_cast<uint32_t>(data[i + 2u]) << 4u);
      
      // Second group of 3 bytes -> 2 values
      uint32_t a1 = static_cast<uint32_t>(data[i + 3u]) | 
                    ((static_cast<uint32_t>(data[i + 4u]) & 0x0fu) << 8u);
      uint32_t b1 = (static_cast<uint32_t>(data[i + 4u]) >> 4u) | 
                    (static_cast<uint32_t>(data[i + 5u]) << 4u);
      
      // Third group of 3 bytes -> 2 values
      uint32_t a2 = static_cast<uint32_t>(data[i + 6u]) | 
                    ((static_cast<uint32_t>(data[i + 7u]) & 0x0fu) << 8u);
      uint32_t b2 = (static_cast<uint32_t>(data[i + 7u]) >> 4u) | 
                    (static_cast<uint32_t>(data[i + 8u]) << 4u);
      
      // Fourth group of 3 bytes -> 2 values
      uint32_t a3 = static_cast<uint32_t>(data[i + 9u]) | 
                    ((static_cast<uint32_t>(data[i + 10u]) & 0x0fu) << 8u);
      uint32_t b3 = (static_cast<uint32_t>(data[i + 10u]) >> 4u) | 
                    (static_cast<uint32_t>(data[i + 11u]) << 4u);
      
      // Branchless accumulation using conditional moves
      local_sum += (a0 > thresh) ? a0 : 0u;
      local_sum += (b0 > thresh) ? b0 : 0u;
      local_sum += (a1 > thresh) ? a1 : 0u;
      local_sum += (b1 > thresh) ? b1 : 0u;
      local_sum += (a2 > thresh) ? a2 : 0u;
      local_sum += (b2 > thresh) ? b2 : 0u;
      local_sum += (a3 > thresh) ? a3 : 0u;
      local_sum += (b3 > thresh) ? b3 : 0u;
    }
    
    // Handle remaining bytes
    for (; i + 2u < size; i += 3u) {
      uint32_t a = static_cast<uint32_t>(data[i]) | 
                   ((static_cast<uint32_t>(data[i + 1u]) & 0x0fu) << 8u);
      uint32_t b = (static_cast<uint32_t>(data[i + 1u]) >> 4u) | 
                   (static_cast<uint32_t>(data[i + 2u]) << 4u);
      
      local_sum += (a > thresh) ? a : 0u;
      local_sum += (b > thresh) ? b : 0u;
    }
    
    sum = local_sum;
  }
  
  return sum;
}