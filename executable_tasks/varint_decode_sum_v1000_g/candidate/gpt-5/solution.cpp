#include "interface.h"

#include <cstdint>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int /*iters*/) {
  const uint8_t* p = encoded.data();
  const uint8_t* end = p + encoded.size();
  uint64_t sum = 0;

  while (p < end) {
    uint32_t v;
    uint8_t b0 = *p++;
    v = static_cast<uint32_t>(b0 & 0x7Fu);
    if (b0 & 0x80u) {
      uint8_t b1 = *p++;
      v |= static_cast<uint32_t>(b1 & 0x7Fu) << 7;
      if (b1 & 0x80u) {
        uint8_t b2 = *p++;
        v |= static_cast<uint32_t>(b2 & 0x7Fu) << 14;
        if (b2 & 0x80u) {
          uint8_t b3 = *p++;
          v |= static_cast<uint32_t>(b3 & 0x7Fu) << 21;
          if (b3 & 0x80u) {
            uint8_t b4 = *p++;
            v |= static_cast<uint32_t>(b4 & 0x7Fu) << 28;
            // Assuming inputs fit into 32 bits as in baseline.
          }
        }
      }
    }
    sum += static_cast<uint64_t>(v);
  }

  return sum;
}
