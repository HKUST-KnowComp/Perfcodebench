#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  uint64_t sum = 0;
  const uint8_t* const data = encoded.data();
  const uint8_t* const end = data + encoded.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const uint8_t* p = data;
    
    // Process varints directly without storing
    while (p < end) {
      // Fast path for single-byte varints (value < 128)
      if (__builtin_expect((*p & 0x80) == 0, 1)) {
        sum += *p++;
        continue;
      }
      
      // Multi-byte varint decoding
      uint64_t value = *p & 0x7F;
      if (__builtin_expect((p[1] & 0x80) == 0, 1)) {
        value |= static_cast<uint64_t>(p[1]) << 7;
        p += 2;
        sum += value;
        continue;
      }
      
      value |= static_cast<uint64_t>(p[1] & 0x7F) << 7;
      if (__builtin_expect((p[2] & 0x80) == 0, 1)) {
        value |= static_cast<uint64_t>(p[2]) << 14;
        p += 3;
        sum += value;
        continue;
      }
      
      value |= static_cast<uint64_t>(p[2] & 0x7F) << 14;
      if (__builtin_expect((p[3] & 0x80) == 0, 1)) {
        value |= static_cast<uint64_t>(p[3]) << 21;
        p += 4;
        sum += value;
        continue;
      }
      
      value |= static_cast<uint64_t>(p[3] & 0x7F) << 21;
      if (__builtin_expect((p[4] & 0x80) == 0, 1)) {
        value |= static_cast<uint64_t>(p[4]) << 28;
        p += 5;
        sum += value;
        continue;
      }
      
      // Handle up to 10 bytes for full uint64
      value |= static_cast<uint64_t>(p[4] & 0x7F) << 28;
      p += 5;
      uint32_t shift = 35;
      while (*p & 0x80) {
        value |= static_cast<uint64_t>(*p & 0x7F) << shift;
        shift += 7;
        ++p;
      }
      value |= static_cast<uint64_t>(*p) << shift;
      ++p;
      sum += value;
    }
  }
  return sum;
}