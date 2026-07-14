#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  if (iters <= 0) return 0;
  const uint8_t* data = encoded.data();
  std::size_t size = encoded.size();
  std::size_t i = 0;
  uint64_t sum = 0;
  while (i < size) {
    uint32_t value = 0;
    uint32_t shift = 0;
    uint8_t byte;
    do {
      byte = data[i++];
      value |= (byte & 0x7f) << shift;
      shift += 7;
    } while (byte & 0x80);
    sum += value;
  }
  return sum;
}