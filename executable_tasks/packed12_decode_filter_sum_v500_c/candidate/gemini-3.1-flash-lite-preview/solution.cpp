#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  uint64_t total_sum = 0;
  const size_t n = packed.size();
  const uint32_t thresh = static_cast<uint32_t>(threshold);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    const uint8_t* p = packed.data();
    
    // Process 3 bytes at a time to extract two 12-bit integers
    for (size_t i = 0; i + 2 < n; i += 3) {
      const uint32_t b0 = p[i];
      const uint32_t b1 = p[i + 1];
      const uint32_t b2 = p[i + 2];

      const uint32_t val1 = b0 | ((b1 & 0x0F) << 8);
      const uint32_t val2 = (b1 >> 4) | (b2 << 4);

      if (val1 > thresh) current_sum += val1;
      if (val2 > thresh) current_sum += val2;
    }
    total_sum = current_sum;
  }
  return total_sum;
}