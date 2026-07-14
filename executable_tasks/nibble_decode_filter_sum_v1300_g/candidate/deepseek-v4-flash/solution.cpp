#include "interface.h"
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  // If threshold >= 15, no nibble (0-15) can exceed it, so sum is always 0.
  if (threshold >= 15) {
    return 0;
  }

  uint64_t sum = 0;
  const uint8_t* data = packed.data();
  size_t n = packed.size();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (size_t i = 0; i < n; ++i) {
      uint8_t byte = data[i];
      uint32_t low = static_cast<uint32_t>(byte & 0x0F);
      uint32_t high = static_cast<uint32_t>(byte >> 4);
      if (low > threshold) {
        sum += low;
      }
      if (high > threshold) {
        sum += high;
      }
    }
  }
  return sum;
}
