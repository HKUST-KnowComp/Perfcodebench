#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  uint64_t sum = 0;
  const std::size_t n = packed.size();
  const uint8_t* data = packed.data();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      uint8_t b = data[i];
      uint8_t low = b & 0x0F;
      uint8_t high = b >> 4;
      if (low > threshold) sum += low;
      if (high > threshold) sum += high;
    }
  }
  return sum;
}