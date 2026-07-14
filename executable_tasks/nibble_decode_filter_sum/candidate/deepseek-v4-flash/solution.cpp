#include "interface.h"

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  if (iters == 0) return 0;
  uint64_t sum = 0;
  const std::size_t n = packed.size();
  for (std::size_t i = 0; i < n; ++i) {
    uint8_t byte = packed[i];
    uint8_t low = byte & 0x0Fu;
    uint8_t high = byte >> 4u;
    if (low > threshold) sum += low;
    if (high > threshold) sum += high;
  }
  return sum;
}
