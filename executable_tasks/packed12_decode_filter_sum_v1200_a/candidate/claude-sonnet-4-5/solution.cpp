#include "interface.h"

#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const std::size_t size = packed.size();
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    const uint8_t* data = packed.data();
    
    // Process 24 bytes (16 values) at a time
    std::size_t i = 0;
    const std::size_t limit = size - (size % 24);
    
    for (; i < limit; i += 24) {
      // Unroll 8 pairs (16 values)
      uint32_t a0 = static_cast<uint32_t>(data[i]) | ((static_cast<uint32_t>(data[i + 1]) & 0x0fu) << 8u);
      uint32_t b0 = (static_cast<uint32_t>(data[i + 1]) >> 4u) | (static_cast<uint32_t>(data[i + 2]) << 4u);
      
      uint32_t a1 = static_cast<uint32_t>(data[i + 3]) | ((static_cast<uint32_t>(data[i + 4]) & 0x0fu) << 8u);
      uint32_t b1 = (static_cast<uint32_t>(data[i + 4]) >> 4u) | (static_cast<uint32_t>(data[i + 5]) << 4u);
      
      uint32_t a2 = static_cast<uint32_t>(data[i + 6]) | ((static_cast<uint32_t>(data[i + 7]) & 0x0fu) << 8u);
      uint32_t b2 = (static_cast<uint32_t>(data[i + 7]) >> 4u) | (static_cast<uint32_t>(data[i + 8]) << 4u);
      
      uint32_t a3 = static_cast<uint32_t>(data[i + 9]) | ((static_cast<uint32_t>(data[i + 10]) & 0x0fu) << 8u);
      uint32_t b3 = (static_cast<uint32_t>(data[i + 10]) >> 4u) | (static_cast<uint32_t>(data[i + 11]) << 4u);
      
      uint32_t a4 = static_cast<uint32_t>(data[i + 12]) | ((static_cast<uint32_t>(data[i + 13]) & 0x0fu) << 8u);
      uint32_t b4 = (static_cast<uint32_t>(data[i + 13]) >> 4u) | (static_cast<uint32_t>(data[i + 14]) << 4u);
      
      uint32_t a5 = static_cast<uint32_t>(data[i + 15]) | ((static_cast<uint32_t>(data[i + 16]) & 0x0fu) << 8u);
      uint32_t b5 = (static_cast<uint32_t>(data[i + 16]) >> 4u) | (static_cast<uint32_t>(data[i + 17]) << 4u);
      
      uint32_t a6 = static_cast<uint32_t>(data[i + 18]) | ((static_cast<uint32_t>(data[i + 19]) & 0x0fu) << 8u);
      uint32_t b6 = (static_cast<uint32_t>(data[i + 19]) >> 4u) | (static_cast<uint32_t>(data[i + 20]) << 4u);
      
      uint32_t a7 = static_cast<uint32_t>(data[i + 21]) | ((static_cast<uint32_t>(data[i + 22]) & 0x0fu) << 8u);
      uint32_t b7 = (static_cast<uint32_t>(data[i + 22]) >> 4u) | (static_cast<uint32_t>(data[i + 23]) << 4u);
      
      // Branchless accumulation
      local_sum += (a0 > thresh) ? a0 : 0;
      local_sum += (b0 > thresh) ? b0 : 0;
      local_sum += (a1 > thresh) ? a1 : 0;
      local_sum += (b1 > thresh) ? b1 : 0;
      local_sum += (a2 > thresh) ? a2 : 0;
      local_sum += (b2 > thresh) ? b2 : 0;
      local_sum += (a3 > thresh) ? a3 : 0;
      local_sum += (b3 > thresh) ? b3 : 0;
      local_sum += (a4 > thresh) ? a4 : 0;
      local_sum += (b4 > thresh) ? b4 : 0;
      local_sum += (a5 > thresh) ? a5 : 0;
      local_sum += (b5 > thresh) ? b5 : 0;
      local_sum += (a6 > thresh) ? a6 : 0;
      local_sum += (b6 > thresh) ? b6 : 0;
      local_sum += (a7 > thresh) ? a7 : 0;
      local_sum += (b7 > thresh) ? b7 : 0;
    }
    
    // Handle remaining bytes
    for (; i + 2 < size; i += 3) {
      uint32_t a = static_cast<uint32_t>(data[i]) | ((static_cast<uint32_t>(data[i + 1]) & 0x0fu) << 8u);
      uint32_t b = (static_cast<uint32_t>(data[i + 1]) >> 4u) | (static_cast<uint32_t>(data[i + 2]) << 4u);
      
      local_sum += (a > thresh) ? a : 0;
      local_sum += (b > thresh) ? b : 0;
    }
    
    sum = local_sum;
  }
  
  return sum;
}