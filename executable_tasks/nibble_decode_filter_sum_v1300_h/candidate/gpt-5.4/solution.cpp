#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  if (iters <= 0 || packed.empty()) return 0;

  // Decoded values are 4-bit nibbles in [0, 15].
  // If threshold >= 15, no value can pass the filter.
  if (threshold >= 15) return 0;

  const uint8_t* data = packed.data();
  const std::size_t n = packed.size();
  const uint8_t t = threshold;

  // Precompute contribution of each possible packed byte:
  // sum of its two nibbles that are strictly greater than threshold.
  uint16_t lut[256];
  for (unsigned b = 0; b < 256; ++b) {
    const uint8_t lo = static_cast<uint8_t>(b & 0x0Fu);
    const uint8_t hi = static_cast<uint8_t>(b >> 4);
    lut[b] = static_cast<uint16_t>((lo > t ? lo : 0) + (hi > t ? hi : 0));
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;

    std::size_t i = 0;
    const std::size_t unroll = n & ~std::size_t(7);
    for (; i < unroll; i += 8) {
      s += lut[data[i + 0]];
      s += lut[data[i + 1]];
      s += lut[data[i + 2]];
      s += lut[data[i + 3]];
      s += lut[data[i + 4]];
      s += lut[data[i + 5]];
      s += lut[data[i + 6]];
      s += lut[data[i + 7]];
    }
    for (; i < n; ++i) {
      s += lut[data[i]];
    }

    sum = s;
  }
  return sum;
}
