#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  uint64_t sum = 0;
  const uint32_t thresh = static_cast<uint32_t>(threshold);
  const std::size_t packed_len = packed.size();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < packed_len; i += 3U) {
      // Identical decode logic to baseline to preserve correctness
      const uint32_t a = static_cast<uint32_t>(packed[i]) |
          (static_cast<uint32_t>(packed[i + 1U] & 0x0FU) << 8U);
      const uint32_t b = (static_cast<uint32_t>(packed[i + 1U] >> 4U)) |
          (static_cast<uint32_t>(packed[i + 2U]) << 4U);
      
      if (a > thresh) sum += a;
      if (b > thresh) sum += b;
    }
  }
  return sum;
}