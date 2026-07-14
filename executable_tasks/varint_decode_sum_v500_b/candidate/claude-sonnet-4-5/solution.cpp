#include "interface.h"

#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  uint64_t sum = 0;
  const std::size_t size = encoded.size();
  const uint8_t* data = encoded.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;
    
    while (i < size) {
      uint32_t value = 0;
      uint32_t shift = 0;
      
      // Unrolled decode loop with branchless continuation
      uint8_t byte0 = data[i++];
      value = byte0 & 127u;
      if (byte0 & 128u) {
        uint8_t byte1 = data[i++];
        value |= static_cast<uint32_t>(byte1 & 127u) << 7;
        if (byte1 & 128u) {
          uint8_t byte2 = data[i++];
          value |= static_cast<uint32_t>(byte2 & 127u) << 14;
          if (byte2 & 128u) {
            uint8_t byte3 = data[i++];
            value |= static_cast<uint32_t>(byte3 & 127u) << 21;
            if (byte3 & 128u) {
              uint8_t byte4 = data[i++];
              value |= static_cast<uint32_t>(byte4 & 127u) << 28;
            }
          }
        }
      }
      
      local_sum += value;
    }
    
    sum = local_sum;
  }
  
  return sum;
}