#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  if (iters <= 0) return 0;
  uint64_t sum = 0;
  std::size_t n = packed.size();
  uint32_t thresh = static_cast<uint32_t>(threshold);
  for (std::size_t i = 0; i + 2 < n; i += 3) {
    uint8_t b0 = packed[i];
    uint8_t b1 = packed[i+1];
    uint8_t b2 = packed[i+2];
    uint32_t a = static_cast<uint32_t>(b0) | ((static_cast<uint32_t>(b1) & 0x0fu) << 8);
    uint32_t b = static_cast<uint32_t>(b1 >> 4) | (static_cast<uint32_t>(b2) << 4);
    if (a > thresh) sum += a;
    if (b > thresh) sum += b;
  }
  return sum;
}