#include "interface.h"

#include <cstddef>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint8_t* data = packed.data();
  const std::size_t n = packed.size();
  const std::size_t triplets = n / 3u; // process only complete 3-byte groups
  const uint32_t thr = static_cast<uint32_t>(threshold);

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const uint8_t* p = data;

    std::size_t t = 0;
    // Unroll by 4 triplets (12 bytes) per iteration
    for (; t + 4u <= triplets; t += 4u) {
      // Triplet 0
      {
        const uint32_t b0 = p[0];
        const uint32_t b1 = p[1];
        const uint32_t b2 = p[2];
        uint32_t v = b0 | ((b1 & 0x0fu) << 8u);
        sum += (-(uint64_t)(v > thr)) & (uint64_t)v;
        v = (b1 >> 4u) | (b2 << 4u);
        sum += (-(uint64_t)(v > thr)) & (uint64_t)v;
      }
      // Triplet 1
      {
        const uint32_t b0 = p[3];
        const uint32_t b1 = p[4];
        const uint32_t b2 = p[5];
        uint32_t v = b0 | ((b1 & 0x0fu) << 8u);
        sum += (-(uint64_t)(v > thr)) & (uint64_t)v;
        v = (b1 >> 4u) | (b2 << 4u);
        sum += (-(uint64_t)(v > thr)) & (uint64_t)v;
      }
      // Triplet 2
      {
        const uint32_t b0 = p[6];
        const uint32_t b1 = p[7];
        const uint32_t b2 = p[8];
        uint32_t v = b0 | ((b1 & 0x0fu) << 8u);
        sum += (-(uint64_t)(v > thr)) & (uint64_t)v;
        v = (b1 >> 4u) | (b2 << 4u);
        sum += (-(uint64_t)(v > thr)) & (uint64_t)v;
      }
      // Triplet 3
      {
        const uint32_t b0 = p[9];
        const uint32_t b1 = p[10];
        const uint32_t b2 = p[11];
        uint32_t v = b0 | ((b1 & 0x0fu) << 8u);
        sum += (-(uint64_t)(v > thr)) & (uint64_t)v;
        v = (b1 >> 4u) | (b2 << 4u);
        sum += (-(uint64_t)(v > thr)) & (uint64_t)v;
      }
      p += 12; // advance 4 triplets
    }

    // Process remaining triplets
    for (; t < triplets; ++t) {
      const uint32_t b0 = p[0];
      const uint32_t b1 = p[1];
      const uint32_t b2 = p[2];
      uint32_t v = b0 | ((b1 & 0x0fu) << 8u);
      sum += (-(uint64_t)(v > thr)) & (uint64_t)v;
      v = (b1 >> 4u) | (b2 << 4u);
      sum += (-(uint64_t)(v > thr)) & (uint64_t)v;
      p += 3;
    }
  }

  return sum;
}
