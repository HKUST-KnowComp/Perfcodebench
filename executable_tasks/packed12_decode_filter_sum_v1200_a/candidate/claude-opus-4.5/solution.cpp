#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint8_t* __restrict__ data = packed.data();
  const std::size_t size = packed.size();
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;
    
    // Process 12 bytes at a time (8 x 12-bit values)
    const std::size_t fast_end = (size / 12) * 12;
    
    for (; i < fast_end; i += 12) {
      // Decode 8 values from 12 bytes
      const uint32_t b0 = data[i];
      const uint32_t b1 = data[i + 1];
      const uint32_t b2 = data[i + 2];
      const uint32_t b3 = data[i + 3];
      const uint32_t b4 = data[i + 4];
      const uint32_t b5 = data[i + 5];
      const uint32_t b6 = data[i + 6];
      const uint32_t b7 = data[i + 7];
      const uint32_t b8 = data[i + 8];
      const uint32_t b9 = data[i + 9];
      const uint32_t b10 = data[i + 10];
      const uint32_t b11 = data[i + 11];
      
      const uint32_t v0 = b0 | ((b1 & 0x0fu) << 8u);
      const uint32_t v1 = (b1 >> 4u) | (b2 << 4u);
      const uint32_t v2 = b3 | ((b4 & 0x0fu) << 8u);
      const uint32_t v3 = (b4 >> 4u) | (b5 << 4u);
      const uint32_t v4 = b6 | ((b7 & 0x0fu) << 8u);
      const uint32_t v5 = (b7 >> 4u) | (b8 << 4u);
      const uint32_t v6 = b9 | ((b10 & 0x0fu) << 8u);
      const uint32_t v7 = (b10 >> 4u) | (b11 << 4u);
      
      // Branchless accumulation
      local_sum += (v0 > thresh) ? v0 : 0;
      local_sum += (v1 > thresh) ? v1 : 0;
      local_sum += (v2 > thresh) ? v2 : 0;
      local_sum += (v3 > thresh) ? v3 : 0;
      local_sum += (v4 > thresh) ? v4 : 0;
      local_sum += (v5 > thresh) ? v5 : 0;
      local_sum += (v6 > thresh) ? v6 : 0;
      local_sum += (v7 > thresh) ? v7 : 0;
    }
    
    // Handle remaining bytes (in groups of 3)
    for (; i + 2 < size; i += 3) {
      const uint32_t b0 = data[i];
      const uint32_t b1 = data[i + 1];
      const uint32_t b2 = data[i + 2];
      
      const uint32_t v0 = b0 | ((b1 & 0x0fu) << 8u);
      const uint32_t v1 = (b1 >> 4u) | (b2 << 4u);
      
      local_sum += (v0 > thresh) ? v0 : 0;
      local_sum += (v1 > thresh) ? v1 : 0;
    }
    
    sum = local_sum;
  }
  
  return sum;
}