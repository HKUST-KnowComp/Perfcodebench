#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  // Match baseline behavior when iters <= 0
  if (iters <= 0) return 0;

  // If threshold >= 15, no 4-bit value (0..15) can be greater than it
  if (threshold >= 15) return 0;

  // Build a LUT once per call: for each byte, sum contributions of its two nibbles
  uint8_t lut[256];
  const uint8_t t = threshold; // 0..14
  for (int b = 0; b < 256; ++b) {
    const uint8_t lo = static_cast<uint8_t>(b & 0x0F);
    const uint8_t hi = static_cast<uint8_t>(b >> 4);
    uint8_t s = 0;
    if (lo > t) s = static_cast<uint8_t>(s + lo);
    if (hi > t) s = static_cast<uint8_t>(s + hi);
    lut[b] = s;
  }

  uint64_t sum = 0;
  const uint8_t* data = packed.data();
  const std::size_t n = packed.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t acc = 0;

    std::size_t i = 0;
    const std::size_t n8 = n & ~static_cast<std::size_t>(7);
    for (; i < n8; i += 8) {
      acc += lut[data[i + 0]];
      acc += lut[data[i + 1]];
      acc += lut[data[i + 2]];
      acc += lut[data[i + 3]];
      acc += lut[data[i + 4]];
      acc += lut[data[i + 5]];
      acc += lut[data[i + 6]];
      acc += lut[data[i + 7]];
    }
    for (; i < n; ++i) {
      acc += lut[data[i]];
    }

    sum = acc; // preserve baseline behavior: sum from last iteration
  }

  return sum;
}
