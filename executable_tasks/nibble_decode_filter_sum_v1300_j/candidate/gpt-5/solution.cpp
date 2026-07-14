#include "interface.h"

#include <cstddef>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  // Fast path: no 4-bit value exceeds 15, so if threshold >= 15, sum is always 0
  if (threshold >= 15 || iters <= 0 || packed.empty()) {
    return 0;
  }

  // Build nibble include table: value if > threshold, else 0
  uint8_t inc[16];
  for (int v = 0; v < 16; ++v) {
    inc[v] = static_cast<uint8_t>((v > threshold) ? v : 0);
  }

  // Build per-byte lookup table: sum of two qualifying nibbles in the byte
  uint8_t lut[256];
  for (int b = 0; b < 256; ++b) {
    uint8_t lo = static_cast<uint8_t>(b & 0xF);
    uint8_t hi = static_cast<uint8_t>(b >> 4);
    lut[b] = static_cast<uint8_t>(inc[lo] + inc[hi]);
  }

  const uint8_t* data = packed.data();
  const std::size_t n = packed.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    std::size_t i = 0;

    // Unroll by 8 for better throughput
    for (; i + 8 <= n; i += 8) {
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

    sum = s;  // match baseline: sum is the result of the last iteration
  }

  return sum;
}
