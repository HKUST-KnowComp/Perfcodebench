#include "interface.h"
#include <cstdint>
#include <vector>

/**
 * Optimized varint decoding and summation.
 * 
 * Key optimizations:
 * 1. Fused decoding and summation: Avoids allocating and writing to a temporary vector.
 * 2. Raw pointer access: Uses const uint8_t* to traverse the buffer, avoiding bounds checks in the hot loop.
 * 3. Loop unrolling: Manually unrolls the varint decoding logic for up to 5 bytes (max for uint32_t).
 * 4. Branch optimization: Uses the fact that most varints are small to prioritize the 1-byte case.
 */
uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  if (encoded.empty()) return 0;

  const uint8_t* const start_ptr = encoded.data();
  const uint8_t* const end_ptr = start_ptr + encoded.size();
  uint64_t last_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    const uint8_t* ptr = start_ptr;

    while (ptr < end_ptr) {
      uint32_t byte = *ptr++;
      if (!(byte & 0x80)) {
        current_sum += byte;
        continue;
      }

      uint32_t value = byte & 0x7F;
      byte = *ptr++;
      value |= (byte & 0x7F) << 7;
      if (!(byte & 0x80)) {
        current_sum += value;
        continue;
      }

      byte = *ptr++;
      value |= (byte & 0x7F) << 14;
      if (!(byte & 0x80)) {
        current_sum += value;
        continue;
      }

      byte = *ptr++;
      value |= (byte & 0x7F) << 21;
      if (!(byte & 0x80)) {
        current_sum += value;
        continue;
      }

      byte = *ptr++;
      value |= (static_cast<uint32_t>(byte & 0x7F) << 28);
      current_sum += value;
      // Note: Standard uint32 varints are max 5 bytes. 
      // If the 5th byte has a continuation bit, it's technically an overflow or 64-bit varint.
      // Given the baseline uses uint32_t, we assume 32-bit range.
    }
    last_sum = current_sum;
  }

  return last_sum;
}