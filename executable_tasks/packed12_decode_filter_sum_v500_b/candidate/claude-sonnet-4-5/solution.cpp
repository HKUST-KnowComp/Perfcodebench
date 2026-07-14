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
      
      // Branchless filtering and accumulation
      local_sum += (v0 > thresh) ? static_cast<uint64_t>(v0) : 0;
      local_sum += (v1 > thresh) ? static_cast<uint64_t>(v1) : 0;
      local_sum += (v2 > thresh) ? static_cast<uint64_t>(v2) : 0;
      local_sum += (v3 > thresh) ? static_cast<uint64_t>(v3) : 0;
      local_sum += (v4 > thresh) ? static_cast<uint64_t>(v4) : 0;
      local_sum += (v5 > thresh) ? static_cast<uint64_t>(v5) : 0;
      local_sum += (v6 > thresh) ? static_cast<uint64_t>(v6) : 0;
      local_sum += (v7 > thresh) ? static_cast<uint64_t>(v7) : 0;
    }
    
    // Handle remaining bytes (3-byte chunks)
    for (; i < size; i += 3) {
      const uint32_t v0 = static_cast<uint32_t>(data[i]) | 
                          (static_cast<uint32_t>(data[i + 1] & 0x0f) << 8);
      const uint32_t v1 = (static_cast<uint32_t>(data[i + 1] >> 4)) | 
                          (static_cast<uint32_t>(data[i + 2]) << 4);
      
      local_sum += (v0 > thresh) ? static_cast<uint64_t>(v0) : 0;
      local_sum += (v1 > thresh) ? static_cast<uint64_t>(v1) : 0;
    }
    
    sum = local_sum;
  }
  
  return sum;
}