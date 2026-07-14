#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  const uint8_t* const base = encoded.data();
  const uint8_t* const end = base + encoded.size();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = base;
    uint64_t s = 0;
    while (p < end) {
      uint64_t value = 0;
      unsigned shift = 0;
      uint8_t byte;
      do {
        byte = *p++;
        value |= (static_cast<uint64_t>(byte & 0x7Fu) << shift);
        shift += 7;
      } while (byte & 0x80u);
      s += value;
    }
    sum = s;
  }

  return sum;
}
