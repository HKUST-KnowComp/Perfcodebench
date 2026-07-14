#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int /*iters*/) {
  const uint8_t* p = encoded.data();
  const uint8_t* end = p + encoded.size();
  uint64_t sum = 0;

  while (p < end) {
    uint64_t value;
    uint8_t b = *p++;
    if ((b & 0x80u) == 0u) {
      sum += static_cast<uint64_t>(b);
      continue;
    }
    value = static_cast<uint64_t>(b & 0x7Fu);

    b = *p++;
    value |= static_cast<uint64_t>(b & 0x7Fu) << 7;
    if ((b & 0x80u) == 0u) { sum += value; continue; }

    b = *p++;
    value |= static_cast<uint64_t>(b & 0x7Fu) << 14;
    if ((b & 0x80u) == 0u) { sum += value; continue; }

    b = *p++;
    value |= static_cast<uint64_t>(b & 0x7Fu) << 21;
    if ((b & 0x80u) == 0u) { sum += value; continue; }

    b = *p++;
    value |= static_cast<uint64_t>(b & 0x7Fu) << 28;
    if ((b & 0x80u) == 0u) { sum += value; continue; }

    // Fallback for unusually large values (>32-bit varints). Handle up to 64-bit.
    unsigned shift = 35;
    for (;;) {
      b = *p++;
      value |= static_cast<uint64_t>(b & 0x7Fu) << shift;
      if ((b & 0x80u) == 0u) break;
      shift += 7;
    }
    sum += value;
  }

  return sum;
}
