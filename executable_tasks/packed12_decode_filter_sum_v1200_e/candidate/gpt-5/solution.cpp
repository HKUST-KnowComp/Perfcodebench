#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const std::size_t n3 = (packed.size() / 3u) * 3u; // process full 3-byte groups only
  const uint8_t* p = packed.data();
  const uint32_t thr = static_cast<uint32_t>(threshold);

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;

    std::size_t i = 0;
    const std::size_t limit = (n3 / 6u) * 6u; // unroll by 2 groups (6 bytes)
    for (; i < limit; i += 6u) {
      // First 3-byte group
      uint32_t b0 = p[i];
      uint32_t b1 = p[i + 1u];
      uint32_t b2 = p[i + 2u];
      uint32_t a0 = (b0 | (b1 << 8)) & 0x0FFFu;     // low 12 bits of b0..b1
      uint32_t c0 = (b1 | (b2 << 8)) >> 4;          // high 12 bits of b1..b2
      if (a0 > thr) s += a0;
      if (c0 > thr) s += c0;

      // Second 3-byte group
      uint32_t b3 = p[i + 3u];
      uint32_t b4 = p[i + 4u];
      uint32_t b5 = p[i + 5u];
      uint32_t a1 = (b3 | (b4 << 8)) & 0x0FFFu;
      uint32_t c1 = (b4 | (b5 << 8)) >> 4;
      if (a1 > thr) s += a1;
      if (c1 > thr) s += c1;
    }

    // Handle remaining single group if any
    for (; i < n3; i += 3u) {
      uint32_t b0 = p[i];
      uint32_t b1 = p[i + 1u];
      uint32_t b2 = p[i + 2u];
      uint32_t a = (b0 | (b1 << 8)) & 0x0FFFu;
      uint32_t c = (b1 | (b2 << 8)) >> 4;
      if (a > thr) s += a;
      if (c > thr) s += c;
    }

    sum = s; // match baseline: return sum from last iteration
  }

  return sum;
}
