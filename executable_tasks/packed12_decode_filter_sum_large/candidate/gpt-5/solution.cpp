#include "interface.h"

#include <vector>
#include <cstddef>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint8_t* data = packed.data();
  const std::size_t ntriplets = packed.size() / 3u;
  const uint32_t thr32 = static_cast<uint32_t>(threshold);

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = data;
    uint64_t local_sum = 0;

    std::size_t j = 0;
    std::size_t unroll_end = ntriplets & ~static_cast<std::size_t>(3);
    for (; j < unroll_end; j += 4) {
      // Triplet 0
      uint32_t w0 = static_cast<uint32_t>(p[0]) |
                    (static_cast<uint32_t>(p[1]) << 8) |
                    (static_cast<uint32_t>(p[2]) << 16);
      p += 3;
      uint32_t a0 = w0 & 0x0FFFu;
      uint32_t b0 = (w0 >> 12) & 0x0FFFu;
      if (a0 > thr32) local_sum += a0;
      if (b0 > thr32) local_sum += b0;

      // Triplet 1
      uint32_t w1 = static_cast<uint32_t>(p[0]) |
                    (static_cast<uint32_t>(p[1]) << 8) |
                    (static_cast<uint32_t>(p[2]) << 16);
      p += 3;
      uint32_t a1 = w1 & 0x0FFFu;
      uint32_t b1 = (w1 >> 12) & 0x0FFFu;
      if (a1 > thr32) local_sum += a1;
      if (b1 > thr32) local_sum += b1;

      // Triplet 2
      uint32_t w2 = static_cast<uint32_t>(p[0]) |
                    (static_cast<uint32_t>(p[1]) << 8) |
                    (static_cast<uint32_t>(p[2]) << 16);
      p += 3;
      uint32_t a2 = w2 & 0x0FFFu;
      uint32_t b2 = (w2 >> 12) & 0x0FFFu;
      if (a2 > thr32) local_sum += a2;
      if (b2 > thr32) local_sum += b2;

      // Triplet 3
      uint32_t w3 = static_cast<uint32_t>(p[0]) |
                    (static_cast<uint32_t>(p[1]) << 8) |
                    (static_cast<uint32_t>(p[2]) << 16);
      p += 3;
      uint32_t a3 = w3 & 0x0FFFu;
      uint32_t b3 = (w3 >> 12) & 0x0FFFu;
      if (a3 > thr32) local_sum += a3;
      if (b3 > thr32) local_sum += b3;
    }

    for (; j < ntriplets; ++j) {
      uint32_t w = static_cast<uint32_t>(p[0]) |
                   (static_cast<uint32_t>(p[1]) << 8) |
                   (static_cast<uint32_t>(p[2]) << 16);
      p += 3;
      uint32_t a = w & 0x0FFFu;
      uint32_t b = (w >> 12) & 0x0FFFu;
      if (a > thr32) local_sum += a;
      if (b > thr32) local_sum += b;
    }

    sum = local_sum; // match baseline: return value from last iteration
  }

  return sum;
}
