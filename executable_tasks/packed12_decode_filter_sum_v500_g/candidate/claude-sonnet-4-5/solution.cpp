#include "interface.h"

#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  uint64_t sum = 0;
  const std::size_t size = packed.size();
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    
    // Process 12 bytes (8 values) at a time for better ILP
    std::size_t i = 0;
    const std::size_t limit = (size / 12) * 12;
    
    for (; i < limit; i += 12) {
      // First group of 4 values (6 bytes)
      const uint32_t a0 = static_cast<uint32_t>(packed[i]) | 
                          (static_cast<uint32_t>(packed[i + 1] & 0x0f) << 8);
      const uint32_t b0 = (static_cast<uint32_t>(packed[i + 1] >> 4)) | 
                          (static_cast<uint32_t>(packed[i + 2]) << 4);
      const uint32_t a1 = static_cast<uint32_t>(packed[i + 3]) | 
                          (static_cast<uint32_t>(packed[i + 4] & 0x0f) << 8);
      const uint32_t b1 = (static_cast<uint32_t>(packed[i + 4] >> 4)) | 
                          (static_cast<uint32_t>(packed[i + 5]) << 4);
      
      // Second group of 4 values (6 bytes)
      const uint32_t a2 = static_cast<uint32_t>(packed[i + 6]) | 
                          (static_cast<uint32_t>(packed[i + 7] & 0x0f) << 8);
      const uint32_t b2 = (static_cast<uint32_t>(packed[i + 7] >> 4)) | 
                          (static_cast<uint32_t>(packed[i + 8]) << 4);
      const uint32_t a3 = static_cast<uint32_t>(packed[i + 9]) | 
                          (static_cast<uint32_t>(packed[i + 10] & 0x0f) << 8);
      const uint32_t b3 = (static_cast<uint32_t>(packed[i + 10] >> 4)) | 
                          (static_cast<uint32_t>(packed[i + 11]) << 4);
      
      // Accumulate with conditional adds
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
      const uint32_t a = static_cast<uint32_t>(packed[i]) | 
                         (static_cast<uint32_t>(packed[i + 1] & 0x0f) << 8);
      const uint32_t b = (static_cast<uint32_t>(packed[i + 1] >> 4)) | 
                         (static_cast<uint32_t>(packed[i + 2]) << 4);
      
      if (a > thresh) local_sum += a;
      if (b > thresh) local_sum += b;
    }
    
    sum = local_sum;
  }
  
  return sum;
}