#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  // Build LUT mapping each byte to the sum of its two nibbles that exceed threshold.
  uint8_t lut[256];
  for (int b = 0; b < 256; ++b) {
    uint8_t lo = static_cast<uint8_t>(b & 0x0F);
    uint8_t hi = static_cast<uint8_t>(static_cast<uint8_t>(b) >> 4);
    uint8_t add = 0;
    if (lo > threshold) add = static_cast<uint8_t>(add + lo);
    if (hi > threshold) add = static_cast<uint8_t>(add + hi);
    lut[b] = add;
  }

  uint64_t sum = 0;
  const uint8_t* data = packed.data();
  const std::size_t n = packed.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    const uint8_t* p = data;
    const uint8_t* end = data + n;

    // Unroll by 4 for better ILP and fewer loop overheads
    while (p + 4 <= end) {
      s += static_cast<uint64_t>(
          static_cast<unsigned>(lut[p[0]]) +
          static_cast<unsigned>(lut[p[1]]) +
          static_cast<unsigned>(lut[p[2]]) +
          static_cast<unsigned>(lut[p[3]])
      );
      p += 4;
    }
    while (p < end) {
      s += lut[*p++];
    }

    sum = s; // result of the last iteration
  }

  return sum;
}
