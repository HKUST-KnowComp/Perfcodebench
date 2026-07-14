#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  uint64_t sum = 0;
  const std::size_t total_triplets = packed.size() / 3u;
  const uint32_t thr = static_cast<uint32_t>(threshold);

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = packed.data();
    sum = 0;

    std::size_t n = total_triplets;
    std::size_t n4 = n / 4u;
    while (n4--) {
      // group 1
      uint32_t b0 = static_cast<uint32_t>(p[0]);
      uint32_t b1 = static_cast<uint32_t>(p[1]);
      uint32_t b2 = static_cast<uint32_t>(p[2]);
      uint32_t a = b0 | ((b1 & 0x0Fu) << 8);
      uint32_t b = (b1 >> 4) | (b2 << 4);
      if (a > thr) sum += static_cast<uint64_t>(a);
      if (b > thr) sum += static_cast<uint64_t>(b);

      // group 2
      b0 = static_cast<uint32_t>(p[3]);
      b1 = static_cast<uint32_t>(p[4]);
      b2 = static_cast<uint32_t>(p[5]);
      a = b0 | ((b1 & 0x0Fu) << 8);
      b = (b1 >> 4) | (b2 << 4);
      if (a > thr) sum += static_cast<uint64_t>(a);
      if (b > thr) sum += static_cast<uint64_t>(b);

      // group 3
      b0 = static_cast<uint32_t>(p[6]);
      b1 = static_cast<uint32_t>(p[7]);
      b2 = static_cast<uint32_t>(p[8]);
      a = b0 | ((b1 & 0x0Fu) << 8);
      b = (b1 >> 4) | (b2 << 4);
      if (a > thr) sum += static_cast<uint64_t>(a);
      if (b > thr) sum += static_cast<uint64_t>(b);

      // group 4
      b0 = static_cast<uint32_t>(p[9]);
      b1 = static_cast<uint32_t>(p[10]);
      b2 = static_cast<uint32_t>(p[11]);
      a = b0 | ((b1 & 0x0Fu) << 8);
      b = (b1 >> 4) | (b2 << 4);
      if (a > thr) sum += static_cast<uint64_t>(a);
      if (b > thr) sum += static_cast<uint64_t>(b);

      p += 12;
    }

    n = n % 4u;
    while (n--) {
      uint32_t b0 = static_cast<uint32_t>(p[0]);
      uint32_t b1 = static_cast<uint32_t>(p[1]);
      uint32_t b2 = static_cast<uint32_t>(p[2]);
      uint32_t a = b0 | ((b1 & 0x0Fu) << 8);
      uint32_t b = (b1 >> 4) | (b2 << 4);
      if (a > thr) sum += static_cast<uint64_t>(a);
      if (b > thr) sum += static_cast<uint64_t>(b);
      p += 3;
    }
  }

  return sum;
}
