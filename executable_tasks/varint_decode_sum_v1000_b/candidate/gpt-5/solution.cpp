#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  uint64_t sum = 0;
  const uint8_t* const begin = encoded.empty() ? nullptr : encoded.data();
  const uint8_t* const end = begin + encoded.size();

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = begin;
    uint64_t local_sum = 0;

    while (p < end) {
      uint32_t v;
      uint8_t b = *p++;

      if ((b & 0x80u) == 0u) {
        // Single-byte value
        v = b;
      } else {
        v = static_cast<uint32_t>(b & 0x7Fu);
        b = *p++;
        v |= (static_cast<uint32_t>(b & 0x7Fu) << 7);
        if ((b & 0x80u) != 0u) {
          b = *p++;
          v |= (static_cast<uint32_t>(b & 0x7Fu) << 14);
          if ((b & 0x80u) != 0u) {
            b = *p++;
            v |= (static_cast<uint32_t>(b & 0x7Fu) << 21);
            if ((b & 0x80u) != 0u) {
              b = *p++;
              // Include all 7 bits shifted by 28 to mimic baseline uint32 behavior
              v |= (static_cast<uint32_t>(b & 0x7Fu) << 28);
              // Drain any remaining continuation bytes (beyond 5) to preserve parsing state
              if ((b & 0x80u) != 0u) {
                do {
                  b = *p++;
                } while ((b & 0x80u) != 0u);
              }
            }
          }
        }
      }

      local_sum += static_cast<uint64_t>(v);
    }

    // Match baseline behavior: return the sum from the last iteration
    sum = local_sum;
  }

  return sum;
}
