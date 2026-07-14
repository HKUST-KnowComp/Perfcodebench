#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  uint64_t sum = 0;
  const uint8_t* const data = encoded.data();
  const uint8_t* const end = data + encoded.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const uint8_t* p = data;
    
    while (p < end) {
      const uint8_t b0 = *p;
      
      // Fast path: 1-byte varint (0-127)
      if ((b0 & 0x80) == 0) {
        sum += b0;
        ++p;
        continue;
      }
      
      // Fast path: 2-byte varint (128-16383) - most common case per task title
      const uint8_t b1 = p[1];
      if ((b1 & 0x80) == 0) {
        sum += (b0 & 0x7F) | (static_cast<uint32_t>(b1) << 7);
        p += 2;
        continue;
      }
      
      // Slower path: 3+ byte varints
      uint32_t value = (b0 & 0x7F) | ((b1 & 0x7F) << 7);
      uint32_t shift = 14;
      p += 2;
      
      for (;;) {
        const uint8_t byte = *p++;
        value |= static_cast<uint32_t>(byte & 0x7F) << shift;
        if ((byte & 0x80) == 0) {
          break;
        }
        shift += 7;
      }
      sum += value;
    }
  }
  return sum;
}