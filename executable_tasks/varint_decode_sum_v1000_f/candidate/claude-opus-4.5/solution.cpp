#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  const uint8_t* const data = encoded.data();
  const uint8_t* const end = data + encoded.size();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const uint8_t* ptr = data;

    while (ptr < end) {
      uint8_t byte = *ptr++;
      
      // Fast path: single-byte varint (most common case)
      if (__builtin_expect((byte & 0x80) == 0, 1)) {
        sum += byte;
        continue;
      }

      uint64_t value = byte & 0x7F;
      
      // Second byte
      byte = *ptr++;
      if (__builtin_expect((byte & 0x80) == 0, 1)) {
        value |= static_cast<uint64_t>(byte) << 7;
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(byte & 0x7F) << 7;

      // Third byte
      byte = *ptr++;
      if ((byte & 0x80) == 0) {
        value |= static_cast<uint64_t>(byte) << 14;
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(byte & 0x7F) << 14;

      // Fourth byte
      byte = *ptr++;
      if ((byte & 0x80) == 0) {
        value |= static_cast<uint64_t>(byte) << 21;
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(byte & 0x7F) << 21;

      // Fifth byte (max for 32-bit varint)
      byte = *ptr++;
      value |= static_cast<uint64_t>(byte & 0x7F) << 28;
      sum += value;
    }
  }

  return sum;
}