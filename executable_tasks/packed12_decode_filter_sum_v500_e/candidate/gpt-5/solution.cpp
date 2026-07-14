#include "interface.h"

#include <vector>
#include <cstddef>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint8_t* data = packed.data();
  const std::size_t n3 = (packed.size() / 3u) * 3u; // number of bytes that form complete 3-byte groups
  const uint8_t* end = data + n3;
  const uint32_t thr = static_cast<uint32_t>(threshold);

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = data;
    uint64_t s = 0;

    // Unroll by 4 groups (12 bytes) when possible
    const uint8_t* uend = data + (n3 / 12u) * 12u;
    while (p < uend) {
      {
        uint32_t t = static_cast<uint32_t>(p[0]) |
                     (static_cast<uint32_t>(p[1]) << 8) |
                     (static_cast<uint32_t>(p[2]) << 16);
        uint32_t a = t & 0x0FFFu;
        if (a > thr) s += a;
        uint32_t b = (t >> 12) & 0x0FFFu;
        if (b > thr) s += b;
        p += 3;
      }
      {
        uint32_t t = static_cast<uint32_t>(p[0]) |
                     (static_cast<uint32_t>(p[1]) << 8) |
                     (static_cast<uint32_t>(p[2]) << 16);
        uint32_t a = t & 0x0FFFu;
        if (a > thr) s += a;
        uint32_t b = (t >> 12) & 0x0FFFu;
        if (b > thr) s += b;
        p += 3;
      }
      {
        uint32_t t = static_cast<uint32_t>(p[0]) |
                     (static_cast<uint32_t>(p[1]) << 8) |
                     (static_cast<uint32_t>(p[2]) << 16);
        uint32_t a = t & 0x0FFFu;
        if (a > thr) s += a;
        uint32_t b = (t >> 12) & 0x0FFFu;
        if (b > thr) s += b;
        p += 3;
      }
      {
        uint32_t t = static_cast<uint32_t>(p[0]) |
                     (static_cast<uint32_t>(p[1]) << 8) |
                     (static_cast<uint32_t>(p[2]) << 16);
        uint32_t a = t & 0x0FFFu;
        if (a > thr) s += a;
        uint32_t b = (t >> 12) & 0x0FFFu;
        if (b > thr) s += b;
        p += 3;
      }
    }

    // Handle any remaining complete groups (if n3 not multiple of 12)
    while (p < end) {
      uint32_t t = static_cast<uint32_t>(p[0]) |
                   (static_cast<uint32_t>(p[1]) << 8) |
                   (static_cast<uint32_t>(p[2]) << 16);
      uint32_t a = t & 0x0FFFu;
      if (a > thr) s += a;
      uint32_t b = (t >> 12) & 0x0FFFu;
      if (b > thr) s += b;
      p += 3;
    }

    sum = s;
  }

  return sum;
}
