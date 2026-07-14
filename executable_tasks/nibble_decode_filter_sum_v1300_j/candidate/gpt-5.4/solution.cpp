#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  if (iters <= 0 || packed.empty()) {
    return 0;
  }

  // No 4-bit value can exceed 15.
  if (threshold >= 15) {
    return 0;
  }

  uint64_t sum = 0;

  // Very selective fast path: only value 15 qualifies.
  if (threshold >= 14) {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t s = 0;
      const uint8_t* p = packed.data();
      const uint8_t* end = p + packed.size();
      while (p != end) {
        const uint8_t b = *p++;
        s += static_cast<uint64_t>((b & 0x0Fu) == 0x0Fu) * 15u;
        s += static_cast<uint64_t>((b >> 4) == 0x0Fu) * 15u;
      }
      sum = s;
    }
    return sum;
  }

  // General path: precompute per-byte contribution for this threshold.
  uint8_t lut[256];
  for (unsigned i = 0; i < 256; ++i) {
    const uint8_t lo = static_cast<uint8_t>(i & 0x0Fu);
    const uint8_t hi = static_cast<uint8_t>(i >> 4);
    lut[i] = static_cast<uint8_t>((lo > threshold ? lo : 0u) + (hi > threshold ? hi : 0u));
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    const uint8_t* p = packed.data();
    const uint8_t* end = p + packed.size();

    // Unroll to reduce loop overhead and improve ILP.
    while (end - p >= 8) {
      s += lut[p[0]];
      s += lut[p[1]];
      s += lut[p[2]];
      s += lut[p[3]];
      s += lut[p[4]];
      s += lut[p[5]];
      s += lut[p[6]];
      s += lut[p[7]];
      p += 8;
    }
    while (p != end) {
      s += lut[*p++];
    }

    sum = s;
  }

  return sum;
}
