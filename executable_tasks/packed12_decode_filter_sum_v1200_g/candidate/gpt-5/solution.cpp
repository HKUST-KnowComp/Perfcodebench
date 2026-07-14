#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  // Early exit: no 12-bit value exceeds 0x0FFF
  if (iters <= 0) return 0;
  if (threshold >= 0x0FFFu) return 0;

  const std::size_t groups = packed.size() / 3u; // each group yields 2 values
  const uint8_t* base = packed.data();
  const uint32_t thr = static_cast<uint32_t>(threshold);

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = base;
    uint64_t s = 0;

    std::size_t i = 0;
    // Unroll by 4 groups -> 12 bytes per iteration
    for (; i + 4 <= groups; i += 4) {
      uint32_t w0 = static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) | (static_cast<uint32_t>(p[2]) << 16);
      uint32_t w1 = static_cast<uint32_t>(p[3]) | (static_cast<uint32_t>(p[4]) << 8) | (static_cast<uint32_t>(p[5]) << 16);
      uint32_t w2 = static_cast<uint32_t>(p[6]) | (static_cast<uint32_t>(p[7]) << 8) | (static_cast<uint32_t>(p[8]) << 16);
      uint32_t w3 = static_cast<uint32_t>(p[9]) | (static_cast<uint32_t>(p[10]) << 8) | (static_cast<uint32_t>(p[11]) << 16);
      p += 12;

      uint32_t a0 = w0 & 0x0FFFu; uint32_t b0 = (w0 >> 12);
      uint32_t a1 = w1 & 0x0FFFu; uint32_t b1 = (w1 >> 12);
      uint32_t a2 = w2 & 0x0FFFu; uint32_t b2 = (w2 >> 12);
      uint32_t a3 = w3 & 0x0FFFu; uint32_t b3 = (w3 >> 12);

      uint32_t m;
      m = static_cast<uint32_t>(-(a0 > thr)); s += static_cast<uint64_t>(a0 & m);
      m = static_cast<uint32_t>(-(b0 > thr)); s += static_cast<uint64_t>(b0 & m);
      m = static_cast<uint32_t>(-(a1 > thr)); s += static_cast<uint64_t>(a1 & m);
      m = static_cast<uint32_t>(-(b1 > thr)); s += static_cast<uint64_t>(b1 & m);
      m = static_cast<uint32_t>(-(a2 > thr)); s += static_cast<uint64_t>(a2 & m);
      m = static_cast<uint32_t>(-(b2 > thr)); s += static_cast<uint64_t>(b2 & m);
      m = static_cast<uint32_t>(-(a3 > thr)); s += static_cast<uint64_t>(a3 & m);
      m = static_cast<uint32_t>(-(b3 > thr)); s += static_cast<uint64_t>(b3 & m);
    }

    // Remainder groups
    for (; i < groups; ++i) {
      uint32_t w = static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) | (static_cast<uint32_t>(p[2]) << 16);
      p += 3;
      uint32_t a = w & 0x0FFFu;
      uint32_t b = (w >> 12);
      uint32_t m;
      m = static_cast<uint32_t>(-(a > thr)); s += static_cast<uint64_t>(a & m);
      m = static_cast<uint32_t>(-(b > thr)); s += static_cast<uint64_t>(b & m);
    }

    sum = s; // per original semantics: return value from the last iteration
  }

  return sum;
}
