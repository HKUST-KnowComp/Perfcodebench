#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  if (iters <= 0) return 0;
  if (packed.empty()) return 0;
  if (threshold >= 15u) return 0; // No 4-bit value is > 15

  // Build a byte->sum LUT for this threshold once per call
  uint8_t lut[256];
  const uint8_t t = threshold;
  for (int b = 0; b < 256; ++b) {
    uint8_t lo = static_cast<uint8_t>(b & 0x0F);
    uint8_t hi = static_cast<uint8_t>(b >> 4);
    uint8_t add = 0;
    if (lo > t) add = static_cast<uint8_t>(add + lo);
    if (hi > t) add = static_cast<uint8_t>(add + hi);
    lut[b] = add;
  }

  const uint8_t* data = packed.data();
  const std::size_t n = packed.size();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    std::size_t i = 0;
    const std::size_t limit = n & ~static_cast<std::size_t>(15); // process in blocks of 16
    while (i < limit) {
      s += lut[data[i    ]]; s += lut[data[i +  1]]; s += lut[data[i +  2]]; s += lut[data[i +  3]];
      s += lut[data[i +  4]]; s += lut[data[i +  5]]; s += lut[data[i +  6]]; s += lut[data[i +  7]];
      s += lut[data[i +  8]]; s += lut[data[i +  9]]; s += lut[data[i + 10]]; s += lut[data[i + 11]];
      s += lut[data[i + 12]]; s += lut[data[i + 13]]; s += lut[data[i + 14]]; s += lut[data[i + 15]];
      i += 16;
    }
    for (; i < n; ++i) {
      s += lut[data[i]];
    }
    sum = s;
  }

  return sum;
}
