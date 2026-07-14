#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  uint64_t final_sum = 0;
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  const std::size_t packed_size = packed.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    for (std::size_t i = 0; i < packed_size; ++i) {
      const uint8_t byte = packed[i];
      // Extract and process low nibble
      uint32_t low = byte & 0xFu;
      if (low > thresh) sum += low;
      // Extract and process high nibble
      uint32_t high = (byte >> 4u) & 0xFu;
      if (high > thresh) sum += high;
    }
    final_sum = sum;
  }
  return final_sum;
}