#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  uint64_t total_sum = 0;
  const uint8_t* data = encoded.data();
  const size_t size = encoded.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    size_t i = 0;
    while (i < size) {
      uint64_t value = 0;
      uint32_t shift = 0;
      while (true) {
        uint8_t byte = data[i++];
        value |= static_cast<uint64_t>(byte & 0x7F) << shift;
        if ((byte & 0x80) == 0) {
          break;
        }
        shift += 7;
      }
      current_iter_sum += value;
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}