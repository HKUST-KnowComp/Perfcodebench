#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint32_t thr32 = static_cast<uint32_t>(threshold);
  const uint8_t* data = packed.data();
  const std::size_t nbytes = packed.size();
  const std::size_t triplets_total = nbytes / 3u; // each triplet -> 2 values

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = data;
    std::size_t t = triplets_total;
    uint64_t s = 0;

    // Unroll by 4 triplets (12 bytes => 8 values)
    while (t >= 4u) {
      // Triplet 0
      uint32_t b0 = p[0];
      uint32_t b1 = p[1];
      uint32_t b2 = p[2];
      uint32_t a0 = b0 | ((b1 & 0x0Fu) << 8);
      uint32_t b0v = (b1 >> 4) | (b2 << 4);
      uint32_t m0 = static_cast<uint32_t>(-(a0 > thr32));
      uint32_t m1 = static_cast<uint32_t>(-(b0v > thr32));
      s += static_cast<uint64_t>(a0 & m0);
      s += static_cast<uint64_t>(b0v & m1);

      // Triplet 1
      b0 = p[3];
      b1 = p[4];
      b2 = p[5];
      a0 = b0 | ((b1 & 0x0Fu) << 8);
      b0v = (b1 >> 4) | (b2 << 4);
      m0 = static_cast<uint32_t>(-(a0 > thr32));
      m1 = static_cast<uint32_t>(-(b0v > thr32));
      s += static_cast<uint64_t>(a0 & m0);
      s += static_cast<uint64_t>(b0v & m1);

      // Triplet 2
      b0 = p[6];
      b1 = p[7];
      b2 = p[8];
      a0 = b0 | ((b1 & 0x0Fu) << 8);
      b0v = (b1 >> 4) | (b2 << 4);
      m0 = static_cast<uint32_t>(-(a0 > thr32));
      m1 = static_cast<uint32_t>(-(b0v > thr32));
      s += static_cast<uint64_t>(a0 & m0);
      s += static_cast<uint64_t>(b0v & m1);

      // Triplet 3
      b0 = p[9];
      b1 = p[10];
      b2 = p[11];
      a0 = b0 | ((b1 & 0x0Fu) << 8);
      b0v = (b1 >> 4) | (b2 << 4);
      m0 = static_cast<uint32_t>(-(a0 > thr32));
      m1 = static_cast<uint32_t>(-(b0v > thr32));
      s += static_cast<uint64_t>(a0 & m0);
      s += static_cast<uint64_t>(b0v & m1);

      p += 12;
      t -= 4u;
    }

    // Handle remaining triplets
    while (t--) {
      uint32_t b0 = p[0];
      uint32_t b1 = p[1];
      uint32_t b2 = p[2];
      uint32_t a = b0 | ((b1 & 0x0Fu) << 8);
      uint32_t b = (b1 >> 4) | (b2 << 4);
      uint32_t ma = static_cast<uint32_t>(-(a > thr32));
      uint32_t mb = static_cast<uint32_t>(-(b > thr32));
      s += static_cast<uint64_t>(a & ma);
      s += static_cast<uint64_t>(b & mb);
      p += 3;
    }

    sum = s; // preserve baseline behavior: return sum from last iteration only
  }

  return sum;
}