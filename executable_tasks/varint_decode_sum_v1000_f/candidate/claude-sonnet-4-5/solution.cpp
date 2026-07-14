#include "interface.h"

#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  uint64_t sum = 0;
  const uint8_t* data = encoded.data();
  const std::size_t size = encoded.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;
    
    while (i < size) {
      uint32_t value = 0;
      uint8_t byte;
      
      // Unrolled decode loop for common 1-5 byte varints
      // Byte 1
      byte = data[i++];
      value = byte & 127u;
      if ((byte & 128u) == 0) {
        sum += value;
        continue;
      }
      
      // Byte 2
      byte = data[i++];
      value |= static_cast<uint32_t>(byte & 127u) << 7;
      if ((byte & 128u) == 0) {
        sum += value;
        continue;
      }
      
      // Byte 3
      byte = data[i++];
      value |= static_cast<uint32_t>(byte & 127u) << 14;
      if ((byte & 128u) == 0) {
        sum += value;
        continue;
      }
      
      // Byte 4
      byte = data[i++];
      value |= static_cast<uint32_t>(byte & 127u) << 21;
      if ((byte & 128u) == 0) {
        sum += value;
        continue;
      }
      
      // Byte 5 (final for uint32_t)
      byte = data[i++];
      value |= static_cast<uint32_t>(byte & 127u) << 28;
      sum += value;
    }
  }
  
  return sum;
}