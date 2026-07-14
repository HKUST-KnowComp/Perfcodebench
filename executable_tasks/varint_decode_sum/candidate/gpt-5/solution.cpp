#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int /*iters*/) {
  const uint8_t* p = encoded.empty() ? nullptr : encoded.data();
  const uint8_t* const end = p + encoded.size();
  uint64_t sum = 0;
  while (p < end) {
    uint32_t v = 0;

    uint8_t b = *p++;
    v |= static_cast<uint32_t>(b & 0x7Fu);
    if ((b & 0x80u) == 0) { sum += v; continue; }

    b = *p++;
    v |= static_cast<uint32_t>(b & 0x7Fu) << 7;
    if ((b & 0x80u) == 0) { sum += v; continue; }

    b = *p++;
    v |= static_cast<uint32_t>(b & 0x7Fu) << 14;
    if ((b & 0x80u) == 0) { sum += v; continue; }

    b = *p++;
    v |= static_cast<uint32_t>(b & 0x7Fu) << 21;
    if ((b & 0x80u) == 0) { sum += v; continue; }

    b = *p++;
    v |= static_cast<uint32_t>(b & 0x7Fu) << 28;
    if ((b & 0x80u) == 0) { sum += v; continue; }

    // If the varint uses more than 5 bytes, continue consuming the rest
    // (value remains truncated to 32 bits, matching baseline semantics).
    do {
      b = *p++;
    } while ((b & 0x80u) != 0 && p < end);

    sum += v;
  }
  return sum;
}
