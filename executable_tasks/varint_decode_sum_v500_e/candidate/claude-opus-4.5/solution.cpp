#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  uint64_t sum = 0;
  const uint8_t* const data = encoded.data();
  const uint8_t* const end = data + encoded.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const uint8_t* ptr = data;
    
    while (ptr < end) {
      uint8_t byte = *ptr++;
      
      // Fast path: single byte varint (most common case)
      if (__builtin_expect((byte & 0x80) == 0, 1)) {
        sum += byte;
        continue;
      }
      
      uint64_t value = byte & 0x7F;
      
      byte = *ptr++;
      if (__builtin_expect((byte & 0x80) == 0, 1)) {
        value |= static_cast<uint64_t>(byte) << 7;
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(byte & 0x7F) << 7;
      
      byte = *ptr++;
      if (__builtin_expect((byte & 0x80) == 0, 1)) {
        value |= static_cast<uint64_t>(byte) << 14;
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(byte & 0x7F) << 14;
      
      byte = *ptr++;
      if (__builtin_expect((byte & 0x80) == 0, 1)) {
        value |= static_cast<uint64_t>(byte) << 21;
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(byte & 0x7F) << 21;
      
      byte = *ptr++;
      if (__builtin_expect((byte & 0x80) == 0, 1)) {
        value |= static_cast<uint64_t>(byte) << 28;
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(byte & 0x7F) << 28;
      
      // Handle remaining bytes for 64-bit varints
      uint32_t shift = 35;
      do {
        byte = *ptr++;
        value |= static_cast<uint64_t>(byte & 0x7F) << shift;
        shift += 7;
      } while (byte & 0x80);
      
      sum += value;
    }
  }
  return sum;
}