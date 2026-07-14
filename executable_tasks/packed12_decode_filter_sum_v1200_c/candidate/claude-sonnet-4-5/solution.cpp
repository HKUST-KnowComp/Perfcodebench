#include "interface.h"

#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  uint64_t sum = 0;
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  const std::size_t size = packed.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;
    
    // Process 6 bytes (4 values) at a time for better throughput
    const std::size_t limit = size - (size % 6);
    for (; i < limit; i += 6) {
      // First pair from bytes [i, i+1, i+2]
      const uint32_t a0 = static_cast<uint32_t>(packed[i]) | 
                          ((static_cast<uint32_t>(packed[i + 1]) & 0x0f) << 8);
      const uint32_t b0 = (static_cast<uint32_t>(packed[i + 1]) >> 4) | 
                          (static_cast<uint32_t>(packed[i + 2]) << 4);
      
      // Second pair from bytes [i+3, i+4, i+5]
      const uint32_t a1 = static_cast<uint32_t>(packed[i + 3]) | 
                          ((static_cast<uint32_t>(packed[i + 4]) & 0x0f) << 8);
      const uint32_t b1 = (static_cast<uint32_t>(packed[i + 4]) >> 4) | 
                          (static_cast<uint32_t>(packed[i + 5]) << 4);
      
      // Branchless conditional accumulation
      local_sum += (a0 > thresh) ? a0 : 0;
      local_sum += (b0 > thresh) ? b0 : 0;
      local_sum += (a1 > thresh) ? a1 : 0;
      local_sum += (b1 > thresh) ? b1 : 0;
    }
    
    // Handle remaining bytes (0 or 3 bytes)
    if (i + 2 < size) {
      const uint32_t a = static_cast<uint32_t>(packed[i]) | 
                         ((static_cast<uint32_t>(packed[i + 1]) & 0x0f) << 8);
      const uint32_t b = (static_cast<uint32_t>(packed[i + 1]) >> 4) | 
                         (static_cast<uint32_t>(packed[i + 2]) << 4);
      local_sum += (a > thresh) ? a : 0;
      local_sum += (b > thresh) ? b : 0;
    }
    
    sum = local_sum;
  }
  
  return sum;
}