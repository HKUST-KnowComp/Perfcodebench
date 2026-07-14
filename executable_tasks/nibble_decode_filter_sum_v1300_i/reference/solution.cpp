#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (uint8_t byte : packed) {
      const uint8_t lo = static_cast<uint8_t>(byte & 15u);
      const uint8_t hi = static_cast<uint8_t>(byte >> 4u);
      if (lo > threshold) {
        sum += static_cast<uint64_t>(lo);
      }
      if (hi > threshold) {
        sum += static_cast<uint64_t>(hi);
      }
    }
  }
  return sum;
}
