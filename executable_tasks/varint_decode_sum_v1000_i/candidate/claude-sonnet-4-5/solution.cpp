#include "interface.h"

#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  if (encoded.empty()) {
    return 0;
  }
  
  const uint8_t* data = encoded.data();
  const uint8_t* end = data + encoded.size();
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* ptr = data;
    
    while (ptr < end) {
      uint8_t byte = *ptr++;
      
      // Fast path: single-byte varint (most common for small values)
      if ((byte & 128u) == 0u) {
        sum += byte;
        continue;
      }
      
      // Multi-byte varint
      uint32_t value = byte & 127u;
      uint32_t shift = 7u;
      
      while (true) {
        byte = *ptr++;
        value |= static_cast<uint32_t>(byte & 127u) << shift;
        if ((byte & 128u) == 0u) {
          break;
        }
        shift += 7u;
      }
      
      sum += value;
    }
  }
  
  return sum;
}