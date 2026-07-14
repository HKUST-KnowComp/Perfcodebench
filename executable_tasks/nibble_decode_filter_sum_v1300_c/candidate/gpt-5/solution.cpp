#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  const std::size_t n = packed.size();
  if (iters <= 0 || n == 0) {
    return 0ULL;
  }
  if (threshold >= 15u) {
    return 0ULL;
  }

  // Precompute contribution per byte for the given threshold.
  uint8_t lut[256];
  for (int b = 0; b < 256; ++b) {
    uint8_t low = static_cast<uint8_t>(b & 0x0Fu);
    uint8_t high = static_cast<uint8_t>(b >> 4);
    uint8_t s = 0;
    if (low > threshold) s += low;
    if (high > threshold) s += high;
    lut[b] = s;
  }

  const uint8_t* data = packed.data();
  uint64_t sum = 0ULL;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0ULL;
    std::size_t i = 0;
    std::size_t limit = n & ~(std::size_t)7; // process 8 bytes per iteration
    for (; i < limit; i += 8) {
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
    sum = s; // final sum from this iteration
  }

  return sum;
}
