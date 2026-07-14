#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  const uint8_t* data = encoded.data();
  std::size_t size = encoded.size();
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;
    while (i < size) {
      uint32_t value = 0;
      uint32_t shift = 0;
      for (;;) {
        uint8_t byte = data[i++];
        value |= static_cast<uint32_t>(byte & 0x7Fu) << shift;
        if ((byte & 0x80u) == 0) break;
        shift += 7u;
      }
      sum += value;
    }
  }
  return sum;
}