#include "interface.h"

#include <cstdint>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  uint64_t sum = 0;
  
  if (encoded.empty()) {
    return 0;
  }
  
  const uint8_t* const data_start = encoded.data();
  const uint8_t* const data_end = data_start + encoded.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    const uint8_t* ptr = data_start;
    
    while (ptr < data_end) {
      // Fast path: single byte varint (values 0-127)
      uint8_t b0 = *ptr++;
      if (__builtin_expect((b0 & 0x80) == 0, 1)) {
        local_sum += b0;
        continue;
      }
      
      // Multi-byte varint
      uint64_t value = b0 & 0x7F;
      
      uint8_t b1 = *ptr++;
      if (__builtin_expect((b1 & 0x80) == 0, 1)) {
        value |= (uint64_t)b1 << 7;
        local_sum += value;
        continue;
      }
      value |= (uint64_t)(b1 & 0x7F) << 7;
      
      uint8_t b2 = *ptr++;
      if (__builtin_expect((b2 & 0x80) == 0, 1)) {
        value |= (uint64_t)b2 << 14;
        local_sum += value;
        continue;
      }
      value |= (uint64_t)(b2 & 0x7F) << 14;
      
      uint8_t b3 = *ptr++;
      if (__builtin_expect((b3 & 0x80) == 0, 1)) {
        value |= (uint64_t)b3 << 21;
        local_sum += value;
        continue;
      }
      value |= (uint64_t)(b3 & 0x7F) << 21;
      
      uint8_t b4 = *ptr++;
      if (__builtin_expect((b4 & 0x80) == 0, 1)) {
        value |= (uint64_t)b4 << 28;
        local_sum += value;
        continue;
      }
      value |= (uint64_t)(b4 & 0x7F) << 28;
      
      // Handle remaining bytes for larger varints (up to 10 bytes for uint64)
      uint32_t shift = 35;
      for (;;) {
        uint8_t byte = *ptr++;
        value |= (uint64_t)(byte & 0x7F) << shift;
        if ((byte & 0x80) == 0) {
          break;
        }
        shift += 7;
      }
      local_sum += value;
    }
    
    sum = local_sum;
  }
  
  return sum;
}