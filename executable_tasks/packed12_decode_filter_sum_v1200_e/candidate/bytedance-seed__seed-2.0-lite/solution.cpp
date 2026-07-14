#include "interface.h"

#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  uint64_t sum = 0;
  const std::size_t packed_size = packed.size();
  const uint32_t threshold_u32 = static_cast<uint32_t>(threshold);
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i + 2 < packed_size; i += 3) {
      const uint32_t a = static_cast<uint32_t>(packed[i]) | 
                         (static_cast<uint32_t>(packed[i+1] & 0x0Fu) << 8u);
      const uint32_t b = (static_cast<uint32_t>(packed[i+1] >> 4u)) | 
                         (static_cast<uint32_t>(packed[i+2]) << 4u);
      
      if (a > threshold_u32) sum += a;
      if (b > threshold_u32) sum += b;
    }
  }
  return sum;
}