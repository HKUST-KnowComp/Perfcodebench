#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  // Number of 12-bit values: 2 per 3 bytes
  std::size_t num_vals = (packed.size() / 3) * 2;
  std::vector<uint32_t> decoded(num_vals);
  
  // Decode once into preallocated vector
  const std::uint8_t* src = packed.data();
  for (std::size_t i = 0; i < packed.size(); i += 3) {
    std::size_t dst_idx = (i / 3) * 2;
    std::uint32_t b0 = src[i];
    std::uint32_t b1 = src[i+1];
    std::uint32_t b2 = src[i+2];
    decoded[dst_idx]     = b0 | ((b1 & 0x0F) << 8);
    decoded[dst_idx + 1] = (b1 >> 4) | (b2 << 4);
  }
  
  // Reuse decoded values across iterations
  uint64_t sum = 0;
  const uint32_t thresh = threshold;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (auto v : decoded) {
      if (v > thresh) sum += v;
    }
  }
  return sum;
}
