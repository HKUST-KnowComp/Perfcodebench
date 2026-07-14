#include "interface.h"

#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  const std::size_t size = packed.size();
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;
    
    // Process 4 bytes at a time (8 nibbles)
    const std::size_t unroll_limit = size & ~3ULL;
    for (; i < unroll_limit; i += 4) {
      uint8_t b0 = packed[i];
      uint8_t b1 = packed[i + 1];
      uint8_t b2 = packed[i + 2];
      uint8_t b3 = packed[i + 3];
      
      uint32_t n0 = b0 & 15u;
      uint32_t n1 = b0 >> 4u;
      uint32_t n2 = b1 & 15u;
      uint32_t n3 = b1 >> 4u;
      uint32_t n4 = b2 & 15u;
      uint32_t n5 = b2 >> 4u;
      uint32_t n6 = b3 & 15u;
      uint32_t n7 = b3 >> 4u;
      
      local_sum += (n0 > thresh) ? n0 : 0;
      local_sum += (n1 > thresh) ? n1 : 0;
      local_sum += (n2 > thresh) ? n2 : 0;
      local_sum += (n3 > thresh) ? n3 : 0;
      local_sum += (n4 > thresh) ? n4 : 0;
      local_sum += (n5 > thresh) ? n5 : 0;
      local_sum += (n6 > thresh) ? n6 : 0;
      local_sum += (n7 > thresh) ? n7 : 0;
    }
    
    // Handle remaining bytes
    for (; i < size; ++i) {
      uint8_t byte = packed[i];
      uint32_t low = byte & 15u;
      uint32_t high = byte >> 4u;
      
      local_sum += (low > thresh) ? low : 0;
      local_sum += (high > thresh) ? high : 0;
    }
    
    sum = local_sum;
  }
  
  return sum;
}