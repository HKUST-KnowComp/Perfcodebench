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
      // Decode 8 values from 12 bytes
      const uint32_t v0 = static_cast<uint32_t>(data[i]) | 
                          (static_cast<uint32_t>(data[i + 1] & 0x0f) << 8);
      const uint32_t v1 = (static_cast<uint32_t>(data[i + 1] >> 4)) | 
                          (static_cast<uint32_t>(data[i + 2]) << 4);
      const uint32_t v2 = static_cast<uint32_t>(data[i + 3]) | 
                          (static_cast<uint32_t>(data[i + 4] & 0x0f) << 8);
      const uint32_t v3 = (static_cast<uint32_t>(data[i + 4] >> 4)) | 
                          (static_cast<uint32_t>(data[i + 5]) << 4);
      const uint32_t v4 = static_cast<uint32_t>(data[i + 6]) | 
                          (static_cast<uint32_t>(data[i + 7] & 0x0f) << 8);
      const uint32_t v5 = (static_cast<uint32_t>(data[i + 7] >> 4)) | 
                          (static_cast<uint32_t>(data[i + 8]) << 4);
      const uint32_t v6 = static_cast<uint32_t>(data[i + 9]) | 
                          (static_cast<uint32_t>(data[i + 10] & 0x0f) << 8);
      const uint32_t v7 = (static_cast<uint32_t>(data[i + 10] >> 4)) | 
                          (static_cast<uint32_t>(data[i + 11]) << 4);
      
      // Filter and accumulate
      if (v0 > thresh) local_sum += v0;
      if (v1 > thresh) local_sum += v1;
      if (v2 > thresh) local_sum += v2;
      if (v3 > thresh) local_sum += v3;
      if (v4 > thresh) local_sum += v4;
      if (v5 > thresh) local_sum += v5;
      if (v6 > thresh) local_sum += v6;
      if (v7 > thresh) local_sum += v7;
    }
    
    // Handle remaining bytes (0-11 bytes, 0-7 values)
    for (; i + 2 < size; i += 3) {
      const uint32_t v0 = static_cast<uint32_t>(data[i]) | 
                          (static_cast<uint32_t>(data[i + 1] & 0x0f) << 8);
      const uint32_t v1 = (static_cast<uint32_t>(data[i + 1] >> 4)) | 
                          (static_cast<uint32_t>(data[i + 2]) << 4);
      
      if (v0 > thresh) local_sum += v0;
      if (v1 > thresh) local_sum += v1;
    }
    
    sum = local_sum;
  }
  
  return sum;
}