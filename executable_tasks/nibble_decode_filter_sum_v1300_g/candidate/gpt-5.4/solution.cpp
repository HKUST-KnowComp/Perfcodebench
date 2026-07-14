#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  if (iters <= 0 || packed.empty()) {
    return 0;
  }

  // All decoded values are in [0, 15].
  // If threshold >= 15, nothing can pass the filter.
  if (threshold >= 15) {
    return 0;
  }

  // Precompute contribution for each possible packed byte:
  // sum of its two nibbles that are strictly greater than threshold.
  uint64_t lut[256];
  for (int b = 0; b < 256; ++b) {
    const uint8_t lo = static_cast<uint8_t>(b & 0x0F);
    const uint8_t hi = static_cast<uint8_t>(b >> 4);
    uint64_t s = 0;
    if (lo > threshold) s += lo;
    if (hi > threshold) s += hi;
    lut[b] = s;
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;

    const uint8_t* p = packed.data();
    std::size_t n = packed.size();
    std::size_t i = 0;

    // Unroll to reduce loop overhead and improve ILP.
    for (; i + 8 <= n; i += 8) {
      local += lut[p[i + 0]];
      local += lut[p[i + 1]];
      local += lut[p[i + 2]];
      local += lut[p[i + 3]];
      local += lut[p[i + 4]];
      local += lut[p[i + 5]];
      local += lut[p[i + 6]];
      local += lut[p[i + 7]];
    }
    for (; i < n; ++i) {
      local += lut[p[i]];
    }

    sum = local;
  }
  return sum;
}
