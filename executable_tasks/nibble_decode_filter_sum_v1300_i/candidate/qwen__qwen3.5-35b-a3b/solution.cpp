#pragma once

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (uint8_t byte : packed) {
      uint8_t lower = byte & 0x0F;
      uint8_t upper = byte >> 4;
      if (lower > threshold) sum += lower;
      if (upper > threshold) sum += upper;
    }
  }
  return sum;
}