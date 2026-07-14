#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  if (iters == 0) return 0;
  uint64_t sum_once = 0;
  std::size_t n = packed.size();
  const uint8_t* p = packed.data();
  for (std::size_t i = 0; i + 2 < n; i += 3) {
    uint32_t a = static_cast<uint32_t>(p[i]) |
                 (static_cast<uint32_t>(p[i + 1] & 0x0F) << 8);
    uint32_t b = static_cast<uint32_t>(p[i + 1] >> 4) |
                 (static_cast<uint32_t>(p[i + 2]) << 4);
    if (a > threshold) sum_once += a;
    if (b > threshold) sum_once += b;
  }
  return sum_once * static_cast<uint64_t>(iters);
}
