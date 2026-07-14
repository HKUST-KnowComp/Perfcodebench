#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  uint64_t sum = 0;
  const uint8_t* data = packed.data();
  std::size_t n = packed.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      uint8_t b = data[i];
      uint32_t lo = b & 15u;
      uint32_t hi = b >> 4u;
      if (lo > threshold) iter_sum += lo;
      if (hi > threshold) iter_sum += hi;
    }
    sum = iter_sum;
  }
  return sum;
}