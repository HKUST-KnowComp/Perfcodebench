#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  if (iters <= 0 || packed.empty()) {
    return 0;
  }

  // All decoded values are in [0, 15]. If threshold >= 15, nothing can pass.
  if (threshold >= 15) {
    return 0;
  }

  // Precompute contribution of each possible packed byte:
  // sum of its two nibbles that are strictly greater than threshold.
  uint64_t table[256];
  for (int b = 0; b < 256; ++b) {
    const uint8_t lo = static_cast<uint8_t>(b & 0x0F);
    const uint8_t hi = static_cast<uint8_t>(b >> 4);
    uint64_t s = 0;
    if (lo > threshold) s += lo;
    if (hi > threshold) s += hi;
    table[b] = s;
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    const uint8_t* data = packed.data();
    const std::size_t n = packed.size();

    std::size_t i = 0;
    for (; i + 7 < n; i += 8) {
      local_sum += table[data[i + 0]];
      local_sum += table[data[i + 1]];
      local_sum += table[data[i + 2]];
      local_sum += table[data[i + 3]];
      local_sum += table[data[i + 4]];
      local_sum += table[data[i + 5]];
      local_sum += table[data[i + 6]];
      local_sum += table[data[i + 7]];
    }
    for (; i < n; ++i) {
      local_sum += table[data[i]];
    }

    sum = local_sum;
  }

  return sum;
}
