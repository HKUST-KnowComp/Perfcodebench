#include "interface.h"

#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  const std::size_t size = packed.size();
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
    std::size_t i = 0;
    
    // Unrolled loop processing 4 bytes (8 nibbles) at a time
    const std::size_t unroll_limit = size & ~3u;
    for (; i < unroll_limit; i += 4) {
      uint8_t b0 = packed[i];
      uint8_t b1 = packed[i + 1];
      uint8_t b2 = packed[i + 2];
      uint8_t b3 = packed[i + 3];
      
      uint32_t lo0 = b0 & 15u;
      uint32_t hi0 = b0 >> 4u;
      uint32_t lo1 = b1 & 15u;
      uint32_t hi1 = b1 >> 4u;
      uint32_t lo2 = b2 & 15u;
      uint32_t hi2 = b2 >> 4u;
      uint32_t lo3 = b3 & 15u;
      uint32_t hi3 = b3 >> 4u;
      
      acc0 += (lo0 > thresh) ? lo0 : 0;
      acc1 += (hi0 > thresh) ? hi0 : 0;
      acc2 += (lo1 > thresh) ? lo1 : 0;
      acc3 += (hi1 > thresh) ? hi1 : 0;
      acc0 += (lo2 > thresh) ? lo2 : 0;
      acc1 += (hi2 > thresh) ? hi2 : 0;
      acc2 += (lo3 > thresh) ? lo3 : 0;
      acc3 += (hi3 > thresh) ? hi3 : 0;
    }
    
    // Handle remaining bytes
    for (; i < size; ++i) {
      uint8_t b = packed[i];
      uint32_t lo = b & 15u;
      uint32_t hi = b >> 4u;
      acc0 += (lo > thresh) ? lo : 0;
      acc1 += (hi > thresh) ? hi : 0;
    }
    
    sum = acc0 + acc1 + acc2 + acc3;
  }
  
  return sum;
}