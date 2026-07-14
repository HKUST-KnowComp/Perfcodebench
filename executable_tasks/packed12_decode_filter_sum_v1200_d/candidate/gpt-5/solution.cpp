#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint8_t* base = packed.data();
  const std::size_t groups = packed.size() / 3u;  // each group is 3 bytes -> 2 values
  const uint32_t thr = static_cast<uint32_t>(threshold);

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = base;
    std::size_t g = groups;
    uint64_t s = 0;

    // Unroll by 4 groups (12 bytes) per iteration
    while (g >= 4u) {
      for (int u = 0; u < 4; ++u) {
        uint32_t pack24 = static_cast<uint32_t>(p[0]) |
                          (static_cast<uint32_t>(p[1]) << 8u) |
                          (static_cast<uint32_t>(p[2]) << 16u);
        p += 3;
        uint32_t a = pack24 & 0x0FFFu;
        uint32_t b = (pack24 >> 12) & 0x0FFFu;
        s += (a > thr) ? static_cast<uint64_t>(a) : 0ull;
        s += (b > thr) ? static_cast<uint64_t>(b) : 0ull;
      }
      g -= 4u;
    }

    while (g > 0u) {
      uint32_t pack24 = static_cast<uint32_t>(p[0]) |
                        (static_cast<uint32_t>(p[1]) << 8u) |
                        (static_cast<uint32_t>(p[2]) << 16u);
      p += 3;
      uint32_t a = pack24 & 0x0FFFu;
      uint32_t b = (pack24 >> 12) & 0x0FFFu;
      s += (a > thr) ? static_cast<uint64_t>(a) : 0ull;
      s += (b > thr) ? static_cast<uint64_t>(b) : 0ull;
      --g;
    }

    sum = s;  // per contract, return the sum from the last iteration
  }

  return sum;
}
