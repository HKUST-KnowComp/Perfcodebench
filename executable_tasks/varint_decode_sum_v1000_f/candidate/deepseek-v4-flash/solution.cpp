#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  uint64_t total_sum = 0;
  const uint8_t* data = encoded.data();
  std::size_t size = encoded.size();
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* ptr = data;
    const uint8_t* end = data + size;
    uint64_t sum = 0;
    while (ptr < end) {
      uint32_t value = 0;
      uint32_t shift = 0;
      while (true) {
        uint8_t byte = *ptr++;
        value |= static_cast<uint32_t>(byte & 0x7F) << shift;
        if (byte < 0x80) break;
        shift += 7;
      }
      sum += value;
    }
    total_sum = sum;
  }
  return total_sum;
}
