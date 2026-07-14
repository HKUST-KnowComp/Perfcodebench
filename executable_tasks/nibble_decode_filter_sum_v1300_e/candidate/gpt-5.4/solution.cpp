#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  if (iters <= 0 || packed.empty()) {
    return 0;
  }

  // All decoded nibbles are in [0, 15].
  // If threshold >= 15, nothing can pass the filter.
  if (threshold >= 15) {
    return 0;
  }

  // Precompute contribution of each possible packed byte for this threshold:
  // contribution[b] = sum of low/high nibble values that are > threshold.
  uint8_t contrib[256];
  for (unsigned b = 0; b < 256; ++b) {
    const uint8_t lo = static_cast<uint8_t>(b & 0x0Fu);
    const uint8_t hi = static_cast<uint8_t>(b >> 4);
    uint8_t s = 0;
    if (lo > threshold) s = static_cast<uint8_t>(s + lo);
    if (hi > threshold) s = static_cast<uint8_t>(s + hi);
    contrib[b] = s;
  }

  const uint8_t* data = packed.data();
  const std::size_t n = packed.size();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t i = 0;
    const std::size_t unroll = 8;
    const std::size_t limit = n & ~(unroll - 1);

    for (; i < limit; i += unroll) {
      local_sum += contrib[data[i + 0]];
      local_sum += contrib[data[i + 1]];
      local_sum += contrib[data[i + 2]];
      local_sum += contrib[data[i + 3]];
      local_sum += contrib[data[i + 4]];
      local_sum += contrib[data[i + 5]];
      local_sum += contrib[data[i + 6]];
      local_sum += contrib[data[i + 7]];
    }
    for (; i < n; ++i) {
      local_sum += contrib[data[i]];
    }

    sum = local_sum;
  }

  return sum;
}
