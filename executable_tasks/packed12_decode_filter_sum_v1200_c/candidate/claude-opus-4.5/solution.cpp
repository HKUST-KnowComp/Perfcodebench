#include "interface.h"

#include <cstdint>
#include <cstring>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint8_t* __restrict data = packed.data();
  const std::size_t size = packed.size();
  const std::size_t triplets = size / 3u;
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;
    const std::size_t limit = triplets * 3u;
    
    // Process 4 triplets (12 bytes -> 8 values) at a time when possible
    const std::size_t unroll_limit = (triplets / 4u) * 12u;
    
    while (i < unroll_limit) {
      // Triplet 0
      uint32_t b0 = data[i];
      uint32_t b1 = data[i + 1];
      uint32_t b2 = data[i + 2];
      uint32_t a0 = b0 | ((b1 & 0x0fu) << 8u);
      uint32_t a1 = (b1 >> 4u) | (b2 << 4u);
      
      // Triplet 1
      uint32_t b3 = data[i + 3];
      uint32_t b4 = data[i + 4];
      uint32_t b5 = data[i + 5];
      uint32_t a2 = b3 | ((b4 & 0x0fu) << 8u);
      uint32_t a3 = (b4 >> 4u) | (b5 << 4u);
      
      // Triplet 2
      uint32_t b6 = data[i + 6];
      uint32_t b7 = data[i + 7];
      uint32_t b8 = data[i + 8];
      uint32_t a4 = b6 | ((b7 & 0x0fu) << 8u);
      uint32_t a5 = (b7 >> 4u) | (b8 << 4u);
      
      // Triplet 3
      uint32_t b9 = data[i + 9];
      uint32_t b10 = data[i + 10];
      uint32_t b11 = data[i + 11];
      uint32_t a6 = b9 | ((b10 & 0x0fu) << 8u);
      uint32_t a7 = (b10 >> 4u) | (b11 << 4u);
      
      // Branchless accumulation
      local_sum += (a0 > thresh) ? a0 : 0u;
      local_sum += (a1 > thresh) ? a1 : 0u;
      local_sum += (a2 > thresh) ? a2 : 0u;
      local_sum += (a3 > thresh) ? a3 : 0u;
      local_sum += (a4 > thresh) ? a4 : 0u;
      local_sum += (a5 > thresh) ? a5 : 0u;
      local_sum += (a6 > thresh) ? a6 : 0u;
      local_sum += (a7 > thresh) ? a7 : 0u;
      
      i += 12;
    }
    
    // Handle remaining triplets
    while (i < limit) {
      uint32_t b0 = data[i];
      uint32_t b1 = data[i + 1];
      uint32_t b2 = data[i + 2];
      uint32_t a = b0 | ((b1 & 0x0fu) << 8u);
      uint32_t b = (b1 >> 4u) | (b2 << 4u);
      
      local_sum += (a > thresh) ? a : 0u;
      local_sum += (b > thresh) ? b : 0u;
      
      i += 3;
    }
    
    sum = local_sum;
  }
  
  return sum;
}