#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  if (iters <= 0 || packed.empty()) return 0;

  // All decoded values are 4-bit, so thresholds >= 15 can never pass.
  if (threshold >= 15) return 0;

  // Precompute contribution of each possible packed byte:
  // sum of its two nibbles that are strictly greater than threshold.
  uint64_t lut[256];
  for (unsigned b = 0; b < 256; ++b) {
    const uint8_t lo = static_cast<uint8_t>(b & 0x0Fu);
    const uint8_t hi = static_cast<uint8_t>(b >> 4);
    uint64_t s = 0;
    if (lo > threshold) s += lo;
    if (hi > threshold) s += hi;
    lut[b] = s;
  }

  const uint8_t* data = packed.data();
  const std::size_t n = packed.size();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
    std::size_t i = 0;

    // Unroll to reduce loop overhead and expose ILP.
    for (; i + 4 <= n; i += 4) {
      acc0 += lut[data[i]];
      acc1 += lut[data[i + 1]];
      acc2 += lut[data[i + 2]];
      acc3 += lut[data[i + 3]];
    }
    for (; i < n; ++i) {
      acc0 += lut[data[i]];
    }

    sum = (acc0 + acc1) + (acc2 + acc3);
  }

  return sum;
}
