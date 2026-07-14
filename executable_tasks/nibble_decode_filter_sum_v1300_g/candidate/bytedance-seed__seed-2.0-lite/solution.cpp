#include "interface.h"
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  uint64_t sum = 0;
  const uint32_t threshold_u32 = static_cast<uint32_t>(threshold);
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < packed.size(); ++i) {
      const uint8_t b = packed[i];
      const uint32_t low_nibble = static_cast<uint32_t>(b & 15u);
      const uint32_t high_nibble = static_cast<uint32_t>(b >> 4u);
      if (low_nibble > threshold_u32) sum += low_nibble;
      if (high_nibble > threshold_u32) sum += high_nibble;
    }
  }
  return sum;
}