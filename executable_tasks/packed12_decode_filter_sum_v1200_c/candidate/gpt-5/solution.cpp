#include "interface.h"

#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint8_t* data = packed.data();
  const std::size_t ntriples = packed.size() / 3u; // only full groups are valid
  const uint32_t thr = static_cast<uint32_t>(threshold);

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = data;
    sum = 0;

    std::size_t t = 0;
    const std::size_t unroll = 4u; // process 4 triples (12 bytes) per iteration
    const std::size_t n_unrolled = (ntriples / unroll) * unroll;

    for (; t < n_unrolled; t += unroll) {
      // Triple 0
      uint32_t b00 = p[0];
      uint32_t b01 = p[1];
      uint32_t b02 = p[2];
      uint32_t a0 = b00 | ((b01 & 0x0Fu) << 8);
      uint32_t b0 = (b01 >> 4) | (b02 << 4);
      if (a0 > thr) sum += a0;
      if (b0 > thr) sum += b0;

      // Triple 1
      uint32_t b10 = p[3];
      uint32_t b11 = p[4];
      uint32_t b12 = p[5];
      uint32_t a1 = b10 | ((b11 & 0x0Fu) << 8);
      uint32_t b1 = (b11 >> 4) | (b12 << 4);
      if (a1 > thr) sum += a1;
      if (b1 > thr) sum += b1;

      // Triple 2
      uint32_t b20 = p[6];
      uint32_t b21 = p[7];
      uint32_t b22 = p[8];
      uint32_t a2 = b20 | ((b21 & 0x0Fu) << 8);
      uint32_t b2 = (b21 >> 4) | (b22 << 4);
      if (a2 > thr) sum += a2;
      if (b2 > thr) sum += b2;

      // Triple 3
      uint32_t b30 = p[9];
      uint32_t b31 = p[10];
      uint32_t b32 = p[11];
      uint32_t a3 = b30 | ((b31 & 0x0Fu) << 8);
      uint32_t b3 = (b31 >> 4) | (b32 << 4);
      if (a3 > thr) sum += a3;
      if (b3 > thr) sum += b3;

      p += 12;
    }

    for (; t < ntriples; ++t) {
      uint32_t b0 = p[0];
      uint32_t b1 = p[1];
      uint32_t b2 = p[2];
      uint32_t a = b0 | ((b1 & 0x0Fu) << 8);
      uint32_t b = (b1 >> 4) | (b2 << 4);
      if (a > thr) sum += a;
      if (b > thr) sum += b;
      p += 3;
    }
  }

  return sum;
}
