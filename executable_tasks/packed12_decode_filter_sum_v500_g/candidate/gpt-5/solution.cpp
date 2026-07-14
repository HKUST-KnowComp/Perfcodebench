#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint8_t* base = packed.data();
  const std::size_t ntrip = packed.size() / 3u; // number of 3-byte groups
  const uint32_t thr = static_cast<uint32_t>(threshold);

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    const uint8_t* p = base;
    std::size_t n = ntrip;

    // Unrolled processing: 4 triplets (12 bytes) -> 8 values per iteration
    while (n >= 4) {
      uint32_t w0 = static_cast<uint32_t>(p[0]) |
                    (static_cast<uint32_t>(p[1]) << 8) |
                    (static_cast<uint32_t>(p[2]) << 16);
      uint32_t a0 = w0 & 0x0FFFu; if (a0 > thr) s += a0;
      uint32_t b0 = (w0 >> 12) & 0x0FFFu; if (b0 > thr) s += b0;

      uint32_t w1 = static_cast<uint32_t>(p[3]) |
                    (static_cast<uint32_t>(p[4]) << 8) |
                    (static_cast<uint32_t>(p[5]) << 16);
      uint32_t a1 = w1 & 0x0FFFu; if (a1 > thr) s += a1;
      uint32_t b1 = (w1 >> 12) & 0x0FFFu; if (b1 > thr) s += b1;

      uint32_t w2 = static_cast<uint32_t>(p[6]) |
                    (static_cast<uint32_t>(p[7]) << 8) |
                    (static_cast<uint32_t>(p[8]) << 16);
      uint32_t a2 = w2 & 0x0FFFu; if (a2 > thr) s += a2;
      uint32_t b2 = (w2 >> 12) & 0x0FFFu; if (b2 > thr) s += b2;

      uint32_t w3 = static_cast<uint32_t>(p[9]) |
                    (static_cast<uint32_t>(p[10]) << 8) |
                    (static_cast<uint32_t>(p[11]) << 16);
      uint32_t a3 = w3 & 0x0FFFu; if (a3 > thr) s += a3;
      uint32_t b3 = (w3 >> 12) & 0x0FFFu; if (b3 > thr) s += b3;

      p += 12;
      n -= 4;
    }

    // Remainder
    while (n--) {
      uint32_t w = static_cast<uint32_t>(p[0]) |
                   (static_cast<uint32_t>(p[1]) << 8) |
                   (static_cast<uint32_t>(p[2]) << 16);
      uint32_t a = w & 0x0FFFu; if (a > thr) s += a;
      uint32_t b = (w >> 12) & 0x0FFFu; if (b > thr) s += b;
      p += 3;
    }

    sum = s; // as in baseline: return sum of last iteration
  }

  return sum;
}
