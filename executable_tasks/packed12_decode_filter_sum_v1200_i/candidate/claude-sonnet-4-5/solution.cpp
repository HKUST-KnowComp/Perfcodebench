#include "interface.h"

#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const std::size_t size = packed.size();
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    const uint8_t* data = packed.data();
    
    // Process 12 bytes at a time (8 values)
    std::size_t i = 0;
    const std::size_t limit = (size / 12) * 12;
    
    for (; i < limit; i += 12) {
      // First pair from bytes 0-2
      uint32_t a0 = static_cast<uint32_t>(data[i]) | 
                    ((static_cast<uint32_t>(data[i + 1]) & 0x0fu) << 8u);
      uint32_t b0 = (static_cast<uint32_t>(data[i + 1]) >> 4u) | 
                    (static_cast<uint32_t>(data[i + 2]) << 4u);
      
      // Second pair from bytes 3-5
      uint32_t a1 = static_cast<uint32_t>(data[i + 3]) | 
                    ((static_cast<uint32_t>(data[i + 4]) & 0x0fu) << 8u);
      uint32_t b1 = (static_cast<uint32_t>(data[i + 4]) >> 4u) | 
                    (static_cast<uint32_t>(data[i + 5]) << 4u);
      
      // Third pair from bytes 6-8
      uint32_t a2 = static_cast<uint32_t>(data[i + 6]) | 
                    ((static_cast<uint32_t>(data[i + 7]) & 0x0fu) << 8u);
      uint32_t b2 = (static_cast<uint32_t>(data[i + 7]) >> 4u) | 
                    (static_cast<uint32_t>(data[i + 8]) << 4u);
      
      // Fourth pair from bytes 9-11
      uint32_t a3 = static_cast<uint32_t>(data[i + 9]) | 
                    ((static_cast<uint32_t>(data[i + 10]) & 0x0fu) << 8u);
      uint32_t b3 = (static_cast<uint32_t>(data[i + 10]) >> 4u) | 
                    (static_cast<uint32_t>(data[i + 11]) << 4u);
      
      // Accumulate filtered values
      if (a0 > thresh) local_sum += a0;
      if (b0 > thresh) local_sum += b0;
      if (a1 > thresh) local_sum += a1;
      if (b1 > thresh) local_sum += b1;
      if (a2 > thresh) local_sum += a2;
      if (b2 > thresh) local_sum += b2;
      if (a3 > thresh) local_sum += a3;
      if (b3 > thresh) local_sum += b3;
    }
    
    // Handle remaining bytes in groups of 3
    for (; i + 2 < size; i += 3) {
      uint32_t a = static_cast<uint32_t>(data[i]) | 
                   ((static_cast<uint32_t>(data[i + 1]) & 0x0fu) << 8u);
      uint32_t b = (static_cast<uint32_t>(data[i + 1]) >> 4u) | 
                   (static_cast<uint32_t>(data[i + 2]) << 4u);
      
      if (a > thresh) local_sum += a;
      if (b > thresh) local_sum += b;
    }
    
    sum = local_sum;
  }
  
  return sum;
}