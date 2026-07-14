#include "interface.h"
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::size_t i = 0;
    sum = 0;
    while (i < encoded.size()) {
      uint32_t value = 0;
      uint32_t shift = 0;
      for (;;) {
        const uint8_t byte = encoded[i++];
        value |= static_cast<uint32_t>(byte & 127u) << shift;
        if ((byte & 128u) == 0u) {
          break;
        }
        shift += 7u;
      }
      sum += static_cast<uint64_t>(value);
    }
  }
  return sum;
}