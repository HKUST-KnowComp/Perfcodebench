#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* data = encoded.data();
    const uint8_t* const end = data + encoded.size();
    uint64_t iter_sum = 0;
    while (data < end) {
      uint32_t value = 0;
      uint32_t shift = 0;
      while (1) {
        const uint8_t byte = *data++;
        value |= static_cast<uint32_t>(byte & 0x7F) << shift;
        if ((byte & 0x80) == 0) break;
        shift += 7;
      }
      iter_sum += value;
    }
    sum = iter_sum;
  }
  return sum;
}
