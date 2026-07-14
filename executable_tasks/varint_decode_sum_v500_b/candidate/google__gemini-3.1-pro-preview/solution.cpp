#include "interface.h"

#include <cstdint>
#include <vector>

/**
 * Optimized varint decoding with fused aggregation.
 * 
 * Improvements:
 * 1. Fused Summation: Removed the std::vector<uint32_t> to avoid memory allocation and 
 *    redundant passes over the data.
 * 2. Fast Path: Added a specialized check for 1-byte varints (the most common case).
 * 3. Loop Unrolling: Manually unrolled the decoding logic for up to 5-byte varints 
 *    (standard for uint32_t) to reduce loop overhead.
 * 4. Pointer Arithmetic: Used raw pointers for faster access compared to iterator/index access.
 */
uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  if (encoded.empty()) return 0;

  const uint8_t* const start = encoded.data();
  const uint8_t* const end = start + encoded.size();
  uint64_t last_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    const uint8_t* ptr = start;

    while (ptr < end) {
      uint8_t byte = *ptr++;
      
      // Fast path: 1-byte varint (MSB is 0)
      if (!(byte & 0x80)) {
        current_sum += byte;
        continue;
      }

      // Multi-byte varint
      uint32_t value = byte & 0x7f;
      
      // Byte 2
      byte = *ptr++;
      value |= static_cast<uint32_t>(byte & 0x7f) << 7;
      if (!(byte & 0x80)) {
        current_sum += value;
        continue;
      }

      // Byte 3
      byte = *ptr++;
      value |= static_cast<uint32_t>(byte & 0x7f) << 14;
      if (!(byte & 0x80)) {
        current_sum += value;
        continue;
      }

      // Byte 4
      byte = *ptr++;
      value |= static_cast<uint32_t>(byte & 0x7f) << 21;
      if (!(byte & 0x80)) {
        current_sum += value;
        continue;
      }

      // Byte 5 (Max for uint32_t)
      byte = *ptr++;
      value |= static_cast<uint32_t>(byte & 0x0f) << 28;
      current_sum += value;
    }
    last_sum = current_sum;
  }

  return last_sum;
}
