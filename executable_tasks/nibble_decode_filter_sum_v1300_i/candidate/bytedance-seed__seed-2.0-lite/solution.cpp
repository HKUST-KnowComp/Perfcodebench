#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int /*iters*/) {
  uint64_t sum = 0;
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  for (uint8_t byte : packed) {
    const uint32_t low_nibble = byte & 0x0FU;
    const uint32_t high_nibble = byte >> 4U;
    if (low_nibble > thresh) sum += low_nibble;
    if (high_nibble > thresh) sum += high_nibble;
  }
  return sum;
}