#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  const uint8_t* base = encoded.data();
  const uint8_t* end = base + encoded.size();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = base;
    uint64_t s = 0;

    while (p < end) {
      uint8_t b = *p++;
      uint64_t v = static_cast<uint64_t>(b & 0x7fu);
      if (b < 0x80u) { s += v; continue; }

      b = *p++; v |= static_cast<uint64_t>(b & 0x7fu) << 7;  if (b < 0x80u) { s += v; continue; }
      b = *p++; v |= static_cast<uint64_t>(b & 0x7fu) << 14; if (b < 0x80u) { s += v; continue; }
      b = *p++; v |= static_cast<uint64_t>(b & 0x7fu) << 21; if (b < 0x80u) { s += v; continue; }
      b = *p++; v |= static_cast<uint64_t>(b & 0x7fu) << 28; if (b < 0x80u) { s += v; continue; }
      b = *p++; v |= static_cast<uint64_t>(b & 0x7fu) << 35; if (b < 0x80u) { s += v; continue; }
      b = *p++; v |= static_cast<uint64_t>(b & 0x7fu) << 42; if (b < 0x80u) { s += v; continue; }
      b = *p++; v |= static_cast<uint64_t>(b & 0x7fu) << 49; if (b < 0x80u) { s += v; continue; }
      b = *p++; v |= static_cast<uint64_t>(b & 0x7fu) << 56; if (b < 0x80u) { s += v; continue; }
      b = *p++;
      v |= static_cast<uint64_t>(b & 0x01u) << 63; // final 64th bit
      s += v;
    }

    sum = s; // return the sum from the final iteration (benchmark semantics)
  }

  return sum;
}
