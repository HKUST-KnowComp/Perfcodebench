#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  // Handle trivial cases
  if (iters <= 0) return 0;
  if (packed.empty()) return 0;

  // Precompute a small lookup table for the given threshold:
  // For each possible byte value, store the sum of its two nibbles that exceed the threshold.
  alignas(64) uint8_t lut[256];
  unsigned t = static_cast<unsigned>(threshold);
  for (int b = 0; b < 256; ++b) {
    unsigned lo = static_cast<unsigned>(b & 0x0F);
    unsigned hi = static_cast<unsigned>(static_cast<unsigned>(b) >> 4);
    unsigned s = (lo > t ? lo : 0u) + (hi > t ? hi : 0u);
    lut[b] = static_cast<uint8_t>(s);
  }

  const uint8_t* data = packed.data();
  std::size_t n = packed.size();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    std::size_t i = 0;

    // Unroll by 8 for throughput
    std::size_t n8 = n & ~static_cast<std::size_t>(7);
    for (; i < n8; i += 8) {
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

    sum = s; // preserve baseline behavior: return the sum from the last iteration
  }

  return sum;
}
