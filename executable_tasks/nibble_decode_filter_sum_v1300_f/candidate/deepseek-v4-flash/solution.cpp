#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  uint64_t sum = 0;
  const size_t n = packed.size();
  const uint8_t thresh = threshold;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t i = 0;

    // Unrolled: process 4 bytes at a time
    for (; i + 3 < n; i += 4) {
      uint8_t b0 = packed[i];
      uint8_t b1 = packed[i+1];
      uint8_t b2 = packed[i+2];
      uint8_t b3 = packed[i+3];

      uint8_t low0 = b0 & 0xF;
      uint8_t high0 = b0 >> 4;
      if (low0 > thresh) sum += low0;
      if (high0 > thresh) sum += high0;

      uint8_t low1 = b1 & 0xF;
      uint8_t high1 = b1 >> 4;
      if (low1 > thresh) sum += low1;
      if (high1 > thresh) sum += high1;

      uint8_t low2 = b2 & 0xF;
      uint8_t high2 = b2 >> 4;
      if (low2 > thresh) sum += low2;
      if (high2 > thresh) sum += high2;

      uint8_t low3 = b3 & 0xF;
      uint8_t high3 = b3 >> 4;
      if (low3 > thresh) sum += low3;
      if (high3 > thresh) sum += high3;
    }

    // Remaining bytes
    for (; i < n; ++i) {
      uint8_t b = packed[i];
      uint8_t low = b & 0xF;
      uint8_t high = b >> 4;
      if (low > thresh) sum += low;
      if (high > thresh) sum += high;
    }
  }
  return sum;
}
