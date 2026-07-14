#include "interface.h"

#include <cstddef>
#include <cstdint>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  uint64_t sum = 0;
  const std::size_t n = encoded.size();
  if (n == 0) {
    // Even if iter > 0, the baseline returns 0; keep consistent behavior.
    return 0;
  }

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = encoded.data();
    const uint8_t* end = p + n;
    uint64_t local_sum = 0;

    while (p < end) {
      uint8_t b0 = *p++;
      if ((b0 & 0x80u) == 0u) {
        // Fast path: single-byte varint
        local_sum += static_cast<uint64_t>(b0);
      } else {
        uint64_t value = static_cast<uint64_t>(b0 & 0x7Fu);
        unsigned shift = 7u;
        for (;;) {
          uint8_t b = *p++;
          value |= (static_cast<uint64_t>(b & 0x7Fu) << shift);
          if ((b & 0x80u) == 0u) {
            break;
          }
          shift += 7u;
        }
        local_sum += value;
      }
    }

    sum = local_sum; // Match baseline: return the sum from the last iteration
  }

  return sum;
}
