#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  const uint8_t* data = encoded.data();
  const size_t len = encoded.size();
  uint64_t last_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    size_t i = 0;
    while (i < len) {
      uint32_t value = 0;
      uint32_t shift = 0;
      while (true) {
        const uint8_t byte = data[i++];
        value |= static_cast<uint32_t>(byte & 0x7F) << shift;
        if ((byte & 0x80) == 0) break;
        shift += 7;
      }
      sum += static_cast<uint64_t>(value);
    }
    last_sum = sum;
  }
  return last_sum;
}