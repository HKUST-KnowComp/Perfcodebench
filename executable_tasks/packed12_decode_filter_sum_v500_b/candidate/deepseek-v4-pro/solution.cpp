#include "interface.h"

#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int /*iters*/) {
  uint64_t sum = 0;
  const uint8_t* p = packed.data();
  const std::size_t sz = packed.size();
  for (std::size_t i = 0; i < sz; i += 3) {
    const uint16_t a = static_cast<uint16_t>(p[i]) |
                       (static_cast<uint16_t>(p[i + 1] & 0x0Fu) << 8);
    const uint16_t b = static_cast<uint16_t>(p[i + 1] >> 4) |
                       (static_cast<uint16_t>(p[i + 2]) << 4);
    if (a > threshold) sum += a;
    if (b > threshold) sum += b;
  }
  return sum;
}
