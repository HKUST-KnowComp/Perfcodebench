#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint8_t* base = packed.data();
  const std::size_t ntriples = packed.size() / 3u; // number of 3-byte groups
  const uint32_t t = static_cast<uint32_t>(threshold);
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const uint8_t* ptr = base;
    std::size_t i = 0;

    // Unroll the loop by 4 triples (12 bytes) to reduce loop overhead
    const std::size_t n4 = ntriples & ~static_cast<std::size_t>(3);
    for (; i < n4; i += 4) {
      // Triple 0
      uint32_t v24_0 = static_cast<uint32_t>(ptr[0]) |
                       (static_cast<uint32_t>(ptr[1]) << 8) |
                       (static_cast<uint32_t>(ptr[2]) << 16);
      uint32_t a0 = v24_0 & 0xFFFu;
      uint32_t b0 = (v24_0 >> 12) & 0xFFFu;
      sum += (a0 > t) ? a0 : 0u;
      sum += (b0 > t) ? b0 : 0u;
      ptr += 3;

      // Triple 1
      uint32_t v24_1 = static_cast<uint32_t>(ptr[0]) |
                       (static_cast<uint32_t>(ptr[1]) << 8) |
                       (static_cast<uint32_t>(ptr[2]) << 16);
      uint32_t a1 = v24_1 & 0xFFFu;
      uint32_t b1 = (v24_1 >> 12) & 0xFFFu;
      sum += (a1 > t) ? a1 : 0u;
      sum += (b1 > t) ? b1 : 0u;
      ptr += 3;

      // Triple 2
      uint32_t v24_2 = static_cast<uint32_t>(ptr[0]) |
                       (static_cast<uint32_t>(ptr[1]) << 8) |
                       (static_cast<uint32_t>(ptr[2]) << 16);
      uint32_t a2 = v24_2 & 0xFFFu;
      uint32_t b2 = (v24_2 >> 12) & 0xFFFu;
      sum += (a2 > t) ? a2 : 0u;
      sum += (b2 > t) ? b2 : 0u;
      ptr += 3;

      // Triple 3
      uint32_t v24_3 = static_cast<uint32_t>(ptr[0]) |
                       (static_cast<uint32_t>(ptr[1]) << 8) |
                       (static_cast<uint32_t>(ptr[2]) << 16);
      uint32_t a3 = v24_3 & 0xFFFu;
      uint32_t b3 = (v24_3 >> 12) & 0xFFFu;
      sum += (a3 > t) ? a3 : 0u;
      sum += (b3 > t) ? b3 : 0u;
      ptr += 3;
    }

    // Process remaining triples
    for (; i < ntriples; ++i) {
      uint32_t v24 = static_cast<uint32_t>(ptr[0]) |
                     (static_cast<uint32_t>(ptr[1]) << 8) |
                     (static_cast<uint32_t>(ptr[2]) << 16);
      uint32_t a = v24 & 0xFFFu;
      uint32_t b = (v24 >> 12) & 0xFFFu;
      if (a > t) sum += a;
      if (b > t) sum += b;
      ptr += 3;
    }
  }

  return sum;
}
