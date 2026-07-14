#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint32_t threshold_u32 = static_cast<uint32_t>(threshold);
  const size_t num_blocks = packed.size() / 3;
  const uint8_t* const packed_data = packed.data();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (size_t i = 0; i < num_blocks; ++i) {
      const size_t byte_offset = i * 3;
      const uint8_t b0 = packed_data[byte_offset];
      const uint8_t b1 = packed_data[byte_offset + 1];
      const uint8_t b2 = packed_data[byte_offset + 2];

      const uint32_t a = static_cast<uint32_t>(b0) | (static_cast<uint32_t>(b1 & 0x0F) << 8);
      const uint32_t b = (static_cast<uint32_t>(b1 >> 4)) | (static_cast<uint32_t>(b2) << 4);

      if (a > threshold_u32) current_sum += a;
      if (b > threshold_u32) current_sum += b;
    }
    sum = current_sum;
  }
  return sum;
}