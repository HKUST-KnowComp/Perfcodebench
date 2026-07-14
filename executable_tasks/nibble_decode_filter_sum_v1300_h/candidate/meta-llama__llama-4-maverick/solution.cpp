#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  uint64_t sum = 0;
  uint32_t threshold_val = static_cast<uint32_t>(threshold);
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < packed.size(); ++i) {
      uint8_t byte = packed[i];
      uint32_t low_nibble = static_cast<uint32_t>(byte & 15u);
      uint32_t high_nibble = static_cast<uint32_t>(byte >> 4u);
      sum += (low_nibble > threshold_val) ? low_nibble : 0;
      sum += (high_nibble > threshold_val) ? high_nibble : 0;
    }
  }
  return sum;
}