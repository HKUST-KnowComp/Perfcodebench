#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  uint64_t sum = 0;
  const std::size_t size = encoded.size();
  const uint8_t* data = encoded.data();
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    std::size_t i = 0;
    while (i < size) {
      uint32_t value = 0;
      uint32_t shift = 0;
      for (;;) {
        const uint8_t byte = data[i++];
        value |= static_cast<uint32_t>(byte & 127u) << shift;
        if ((byte & 128u) == 0u) break;
        shift += 7u;
      }
      iter_sum += static_cast<uint64_t>(value);
    }
    sum = iter_sum;
  }
  return sum;
}