#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  const uint8_t* base = encoded.data();
  const uint8_t* end = base + encoded.size();
  uint64_t last_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = base;
    uint64_t s = 0;

    while (p < end) {
      uint32_t v;
      uint8_t b;

      b = *p++;
      v = static_cast<uint32_t>(b & 0x7Fu);
      if ((b & 0x80u) == 0u) { s += v; continue; }

      b = *p++;
      v |= static_cast<uint32_t>(b & 0x7Fu) << 7;
      if ((b & 0x80u) == 0u) { s += v; continue; }

      b = *p++;
      v |= static_cast<uint32_t>(b & 0x7Fu) << 14;
      if ((b & 0x80u) == 0u) { s += v; continue; }

      b = *p++;
      v |= static_cast<uint32_t>(b & 0x7Fu) << 21;
      if ((b & 0x80u) == 0u) { s += v; continue; }

      b = *p++;
      v |= static_cast<uint32_t>(b & 0x7Fu) << 28;
      if ((b & 0x80u) == 0u) {
        s += v;
      } else {
        // More than 5 bytes: consume remaining continuation bytes like baseline,
        // effectively truncating to 32 bits while discarding higher bits.
        do {
          b = *p++;
        } while ((b & 0x80u) != 0u);
        s += v;
      }
    }

    last_sum = s;
  }

  return last_sum;
}
