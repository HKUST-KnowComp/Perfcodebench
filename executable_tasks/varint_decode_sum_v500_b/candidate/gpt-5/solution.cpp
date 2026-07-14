#include "interface.h"

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  if (iters <= 0) {
    return 0;
  }
  const uint8_t* p = encoded.data();
  const uint8_t* end = p + encoded.size();
  uint64_t sum = 0;
  while (p < end) {
    uint64_t value = 0;
    uint32_t shift = 0;
    for (;;) {
      uint8_t byte = *p++;
      value |= static_cast<uint64_t>(byte & 0x7Fu) << shift;
      if ((byte & 0x80u) == 0u) {
        break;
      }
      shift += 7u;
    }
    sum += value;
  }
  return sum;
}
