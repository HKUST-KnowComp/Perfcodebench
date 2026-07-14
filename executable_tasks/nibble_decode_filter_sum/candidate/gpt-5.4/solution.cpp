#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  if (iters <= 0 || packed.empty()) return 0;

  // Decoded values are 4-bit nibbles in [0, 15].
  // If threshold >= 15, nothing can pass.
  if (threshold >= 15) return 0;

  const uint8_t* data = packed.data();
  const std::size_t n = packed.size();
  const uint8_t t = threshold;

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;

    for (std::size_t i = 0; i < n; ++i) {
      const uint8_t b = data[i];
      const uint8_t lo = static_cast<uint8_t>(b & 0x0Fu);
      const uint8_t hi = static_cast<uint8_t>(b >> 4);

      if (lo > t) s += lo;
      if (hi > t) s += hi;
    }

    sum = s;
  }

  return sum;
}
