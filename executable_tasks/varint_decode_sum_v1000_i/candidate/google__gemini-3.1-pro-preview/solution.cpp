#include "interface.h"
#include <cstdint>
#include <vector>

/**
 * Optimized varint decoding with fused aggregation.
 * 
 * Improvements:
 * 1. Eliminated std::vector<uint32_t> to avoid heap allocations and memory traffic.
 * 2. Fused decoding and summation into a single loop.
 * 3. Used pointer arithmetic instead of indexing.
 * 4. Implemented a fast-path for the common 1-byte varint case.
 * 5. Unrolled the multi-byte decoding logic to avoid inner loop overhead.
 */
uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  if (encoded.empty()) return 0;

  const uint8_t* const start = encoded.data();
  const uint8_t* const end = start + encoded.size();
  uint64_t last_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    const uint8_t* p = start;

    // Fast path: process most of the buffer without checking bounds for every byte
    // A 32-bit varint can take up to 5 bytes.
    while (p + 5 <= end) {
      uint32_t b = *p++;
      if (!(b & 0x80)) {
        current_sum += b;
        continue;
      }

      uint32_t val = b & 0x7F;
      b = *p++;
      val |= (b & 0x7F) << 7;
      if (!(b & 0x80)) {
        current_sum += val;
        continue;
      }

      b = *p++;
      val |= (b & 0x7F) << 14;
      if (!(b & 0x80)) {
        current_sum += val;
        continue;
      }

      b = *p++;
      val |= (b & 0x7F) << 21;
      if (!(b & 0x80)) {
        current_sum += val;
        continue;
      }

      b = *p++;
      val |= (b & 0x0F) << 28; // Only 4 bits left for a 32-bit value
      current_sum += val;
    }

    // Slow path: handle the remaining bytes near the end of the buffer
    while (p < end) {
      uint32_t val = 0;
      uint32_t shift = 0;
      while (p < end) {
        uint8_t b = *p++;
        val |= static_cast<uint32_t>(b & 0x7F) << shift;
        if (!(b & 0x80)) break;
        shift += 7;
      }
      current_sum += val;
    }

    last_sum = current_sum;
  }

  return last_sum;
}
