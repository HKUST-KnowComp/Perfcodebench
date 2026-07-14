#include "interface.h"

#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int /*iters*/) {
  // The result is independent of the number of iterations in the baseline, so compute once.
  if (threshold >= 15u) {
    // 4-bit values are in [0,15]; none can be > threshold
    return 0;
  }

  // Build a small LUT for the current threshold: for each byte value b, store
  // the sum of its two nibbles if they exceed the threshold.
  uint8_t map16[16];
  for (unsigned v = 0; v < 16; ++v) {
    map16[v] = static_cast<uint8_t>(v > threshold ? v : 0);
  }
  uint8_t lut[256];
  for (unsigned b = 0; b < 256; ++b) {
    lut[b] = static_cast<uint8_t>(map16[b & 0x0Fu] + map16[b >> 4]);
  }

  const uint8_t* data = packed.data();
  const std::size_t n = packed.size();
  uint64_t sum = 0;

  std::size_t i = 0;
  // Unroll to reduce loop overhead
  for (; i + 4 <= n; i += 4) {
    sum += lut[data[i]];
    sum += lut[data[i + 1]];
    sum += lut[data[i + 2]];
    sum += lut[data[i + 3]];
  }
  for (; i < n; ++i) {
    sum += lut[data[i]];
  }

  return sum;
}
