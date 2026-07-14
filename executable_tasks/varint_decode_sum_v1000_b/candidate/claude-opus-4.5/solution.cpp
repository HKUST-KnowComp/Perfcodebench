#include "interface.h"

#include <cstdint>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  uint64_t sum = 0;
  const uint8_t* const data = encoded.data();
  const std::size_t size = encoded.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const uint8_t* ptr = data;
    const uint8_t* const end = data + size;
    
    // Process in bulk while we have enough bytes
    while (ptr + 16 <= end) {
      // Prefetch ahead for better cache performance
      __builtin_prefetch(ptr + 64, 0, 0);
      
      // Fast path: check if next 4 values are single-byte varints
      if ((ptr[0] | ptr[1] | ptr[2] | ptr[3]) < 128) {
        sum += ptr[0] + ptr[1] + ptr[2] + ptr[3];
        ptr += 4;
        continue;
      }
      
      // Decode one varint
      uint64_t value = *ptr & 0x7F;
      if (*ptr++ < 128) {
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(*ptr & 0x7F) << 7;
      if (*ptr++ < 128) {
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(*ptr & 0x7F) << 14;
      if (*ptr++ < 128) {
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(*ptr & 0x7F) << 21;
      if (*ptr++ < 128) {
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(*ptr & 0x7F) << 28;
      if (*ptr++ < 128) {
        sum += value;
        continue;
      }
      // Handle up to 64-bit varints (10 bytes max)
      value |= static_cast<uint64_t>(*ptr & 0x7F) << 35;
      if (*ptr++ < 128) {
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(*ptr & 0x7F) << 42;
      if (*ptr++ < 128) {
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(*ptr & 0x7F) << 49;
      if (*ptr++ < 128) {
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(*ptr & 0x7F) << 56;
      if (*ptr++ < 128) {
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(*ptr & 0x7F) << 63;
      ++ptr;
      sum += value;
    }
    
    // Handle remaining bytes
    while (ptr < end) {
      uint64_t value = *ptr & 0x7F;
      if (*ptr++ < 128) {
        sum += value;
        continue;
      }
      uint32_t shift = 7;
      while (ptr < end) {
        uint8_t byte = *ptr++;
        value |= static_cast<uint64_t>(byte & 0x7F) << shift;
        if (byte < 128) break;
        shift += 7;
      }
      sum += value;
    }
  }
  return sum;
}