#pragma once

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (uint8_t byte : packed) {
      uint8_t low = byte & 0x0F;
      uint8_t high = byte >> 4;
      if (low > threshold) sum += low;
      if (high > threshold) sum += high;
    }
  }
  return sum;
}