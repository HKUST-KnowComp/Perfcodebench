#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  if (iters <= 0) return 0;
  uint64_t sum = 0;
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  const std::size_t sz = packed.size();
  for (std::size_t i = 0; i + 2 < sz; i += 3) {
    const uint8_t b0 = packed[i];
    const uint8_t b1 = packed[i + 1];
    const uint8_t b2 = packed[i + 2];
    const uint32_t a = b0 | ((b1 & 0x0F) << 8);
    const uint32_t b = (b1 >> 4) | (b2 << 4);
    if (a > thresh) sum += a;
    if (b > thresh) sum += b;
  }
  return sum;
}