#pragma once

#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (std::size_t i = 0; i < packed.size(); ++i) {
      uint8_t lower = packed[i] & 0x0F;
      uint8_t upper = packed[i] >> 4;
      if (lower > threshold) iter_sum += lower;
      if (upper > threshold) iter_sum += upper;
    }
    sum += iter_sum;
  }
  return sum;
}