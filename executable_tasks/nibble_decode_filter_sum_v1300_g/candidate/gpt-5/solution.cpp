#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  // Fast path: 4-bit values are in [0,15], so if threshold >= 15, nothing qualifies.
  if (threshold >= 15) {
    return 0;
  }

  // Precompute per-byte contribution: sum of its two nibbles that are > threshold.
  uint16_t lut[256];
  const uint8_t th = threshold;
  for (int b = 0; b < 256; ++b) {
    const uint8_t lo = static_cast<uint8_t>(b & 0x0F);
    const uint8_t hi = static_cast<uint8_t>(b >> 4);
    uint16_t s = 0;
    if (lo > th) s += lo;
    if (hi > th) s += hi;
    lut[b] = s;
  }

  const uint8_t* data = packed.data();
  const std::size_t n = packed.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t acc = 0;
    const uint8_t* p = data;
    const uint8_t* e8 = data + (n & ~static_cast<std::size_t>(7));
    while (p < e8) {
      acc += lut[p[0]];
      acc += lut[p[1]];
      acc += lut[p[2]];
      acc += lut[p[3]];
      acc += lut[p[4]];
      acc += lut[p[5]];
      acc += lut[p[6]];
      acc += lut[p[7]];
      p += 8;
    }
    const uint8_t* e = data + n;
    while (p < e) {
      acc += lut[*p++];
    }
    sum = acc;  // Return the sum from the last iteration (matches baseline behavior).
  }

  return sum;
}
