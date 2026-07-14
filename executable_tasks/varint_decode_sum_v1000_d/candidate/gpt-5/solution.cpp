#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  const uint8_t* data = encoded.data();
  const std::size_t n = encoded.size();
  uint64_t last_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = data;
    const uint8_t* end = data + n;
    uint64_t sum = 0;

    while (p < end) {
      uint8_t b0 = *p++;
      if (b0 < 128) { // single byte
        sum += static_cast<uint64_t>(b0);
        continue;
      }

      // two-byte fast path
      uint32_t v32 = static_cast<uint32_t>(b0 & 0x7Fu);
      uint8_t b1 = *p++;
      v32 |= static_cast<uint32_t>(b1 & 0x7Fu) << 7;
      if (b1 < 128) {
        sum += static_cast<uint64_t>(v32);
        continue;
      }

      // general path for longer varints
      uint64_t v64 = static_cast<uint64_t>(v32);
      unsigned shift = 14;
      for (;;) {
        uint8_t bx = *p++;
        v64 |= (static_cast<uint64_t>(bx & 0x7Fu) << shift);
        if (bx < 128) {
          sum += v64;
          break;
        }
        shift += 7;
      }
    }

    last_sum = sum;
  }

  return last_sum;
}
