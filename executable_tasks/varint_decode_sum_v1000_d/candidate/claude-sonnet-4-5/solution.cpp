#include "interface.h"

#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  uint64_t sum = 0;
  const std::size_t size = encoded.size();
  const uint8_t* data = encoded.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;
    
    while (i < size) {
      const uint8_t byte0 = data[i++];
      
      // Fast path: single byte varint (most significant bit is 0)
      if ((byte0 & 0x80) == 0) {
        sum += byte0;
        continue;
      }
      
      // Fast path: two byte varint
      if (i < size) {
        const uint8_t byte1 = data[i];
        if ((byte1 & 0x80) == 0) {
          i++;
          uint32_t value = (byte0 & 0x7F) | (static_cast<uint32_t>(byte1) << 7);
          sum += value;
          continue;
        }
      }
      
      // Slow path: 3+ byte varint
      uint32_t value = byte0 & 0x7F;
      uint32_t shift = 7;
      
      while (i < size) {
        const uint8_t byte = data[i++];
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
