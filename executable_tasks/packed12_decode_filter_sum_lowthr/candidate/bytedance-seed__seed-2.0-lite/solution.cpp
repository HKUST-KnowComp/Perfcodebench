#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int /*iters*/) {
  uint64_t sum = 0;
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  const std::size_t len = packed.size();
  for (std::size_t i = 0; i < len; i += 3) {
    const uint32_t a = static_cast<uint32_t>(packed[i]) | (static_cast<uint32_t>(packed[i+1] & 0x0FU) << 8U);
    const uint32_t b = (static_cast<uint32_t>(packed[i+1] >> 4U)) | (static_cast<uint32_t>(packed[i+2]) << 4U);
    if (a > thresh) sum += a;
    if (b > thresh) sum += b;
  }
  return sum;
}