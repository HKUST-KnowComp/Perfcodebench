#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  if (iters <= 0 || packed.empty()) {
    return 0;
  }

  // Nibbles are in [0, 15]. If threshold >= 15, nothing can pass.
  if (threshold >= 15) {
    return 0;
  }

  // Precompute contribution of each possible packed byte:
  // sum of its two nibbles that are strictly greater than threshold.
  uint64_t lut[256];
  for (int b = 0; b < 256; ++b) {
    const uint32_t lo = static_cast<uint32_t>(b & 0x0F);
    const uint32_t hi = static_cast<uint32_t>((b >> 4) & 0x0F);
    uint64_t s = 0;
    if (lo > threshold) s += lo;
    if (hi > threshold) s += hi;
    lut[b] = s;
  }

  const uint8_t* data = packed.data();
  const std::size_t n = packed.size();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t acc0 = 0;
    uint64_t acc1 = 0;
    uint64_t acc2 = 0;
    uint64_t acc3 = 0;

    std::size_t i = 0;
    const std::size_t unroll_end = n & ~static_cast<std::size_t>(3);
    for (; i < unroll_end; i += 4) {
      acc0 += lut[data[i]];
      acc1 += lut[data[i + 1]];
      acc2 += lut[data[i + 2]];
      acc3 += lut[data[i + 3]];
    }

    uint64_t iter_sum = acc0 + acc1 + acc2 + acc3;
    for (; i < n; ++i) {
      iter_sum += lut[data[i]];
    }

    sum = iter_sum;
  }

  return sum;
}
