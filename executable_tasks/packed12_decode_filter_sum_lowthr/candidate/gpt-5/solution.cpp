#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint8_t* data = packed.data();
  const std::size_t nbytes = packed.size();
  const std::size_t ntriples = nbytes / 3u;  // number of 3-byte groups
  const uint32_t t = static_cast<uint32_t>(threshold);

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = data;
    std::size_t remaining = ntriples;
    uint64_t s = 0;

    // Process 4 triples (12 bytes) per iteration => 8 values
    while (remaining >= 4u) {
      // Triple 0
      {
        uint32_t v16 = static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8);
        uint32_t a0 = v16 & 0x0FFFu;
        uint32_t a1 = (v16 >> 12) | (static_cast<uint32_t>(p[2]) << 4);
        if (a0 > t) s += a0;
        if (a1 > t) s += a1;
      }
      // Triple 1
      {
        uint32_t v16 = static_cast<uint32_t>(p[3]) | (static_cast<uint32_t>(p[4]) << 8);
        uint32_t a0 = v16 & 0x0FFFu;
        uint32_t a1 = (v16 >> 12) | (static_cast<uint32_t>(p[5]) << 4);
        if (a0 > t) s += a0;
        if (a1 > t) s += a1;
      }
      // Triple 2
      {
        uint32_t v16 = static_cast<uint32_t>(p[6]) | (static_cast<uint32_t>(p[7]) << 8);
        uint32_t a0 = v16 & 0x0FFFu;
        uint32_t a1 = (v16 >> 12) | (static_cast<uint32_t>(p[8]) << 4);
        if (a0 > t) s += a0;
        if (a1 > t) s += a1;
      }
      // Triple 3
      {
        uint32_t v16 = static_cast<uint32_t>(p[9]) | (static_cast<uint32_t>(p[10]) << 8);
        uint32_t a0 = v16 & 0x0FFFu;
        uint32_t a1 = (v16 >> 12) | (static_cast<uint32_t>(p[11]) << 4);
        if (a0 > t) s += a0;
        if (a1 > t) s += a1;
      }

      p += 12;
      remaining -= 4u;
    }

    // Process remaining triples
    while (remaining > 0u) {
      uint32_t v16 = static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8);
      uint32_t a0 = v16 & 0x0FFFu;
      uint32_t a1 = (v16 >> 12) | (static_cast<uint32_t>(p[2]) << 4);
      if (a0 > t) s += a0;
      if (a1 > t) s += a1;
      p += 3;
      --remaining;
    }

    sum = s; // per-contract: return the sum from the last iteration
  }

  return sum;
}
