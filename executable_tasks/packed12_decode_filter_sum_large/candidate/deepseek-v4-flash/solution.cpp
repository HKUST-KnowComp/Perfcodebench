#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  if (iters <= 0) return 0;
  uint64_t sum = 0;
  const std::size_t n = packed.size();
  const uint32_t thr = static_cast<uint32_t>(threshold);
  for (std::size_t i = 0; i + 2 < n; i += 3) {
    const uint32_t byte0 = packed[i];
    const uint32_t byte1 = packed[i + 1];
    const uint32_t byte2 = packed[i + 2];
    const uint32_t a = byte0 | ((byte1 & 0x0Fu) << 8u);
    const uint32_t b = (byte1 >> 4u) | (byte2 << 4u);
    if (a > thr) sum += a;
    if (b > thr) sum += b;
  }
  return sum;
}