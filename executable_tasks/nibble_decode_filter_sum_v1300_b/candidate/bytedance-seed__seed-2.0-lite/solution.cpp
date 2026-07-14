#include "interface.h"

#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  uint64_t final_sum = 0;
  const std::size_t packed_len = packed.size();
  const uint8_t* const packed_data = packed.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (std::size_t i = 0; i < packed_len; ++i) {
      const uint8_t byte = packed_data[i];
      // Process low nibble
      const uint8_t low_nib = byte & 0x0F;
      if (low_nib > threshold) {
        current_sum += low_nib;
      }
      // Process high nibble
      const uint8_t high_nib = byte >> 4;
      if (high_nib > threshold) {
        current_sum += high_nib;
      }
    }
    final_sum = current_sum;
  }
  return final_sum;
}