#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  const uint8_t* data = encoded.data();
  const uint8_t* end = data + encoded.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = data;
    uint64_t s = 0;
    while (p < end) {
      uint64_t value = 0;
      unsigned shift = 0;
      uint8_t byte;
      do {
        byte = *p++;
        value |= static_cast<uint64_t>(byte & 0x7Fu) << shift;
        shift += 7u;
      } while (byte & 0x80u);
      s += value;
    }
    sum = s; // preserve baseline behavior: return sum from the last iteration
  }
  return sum;
}
