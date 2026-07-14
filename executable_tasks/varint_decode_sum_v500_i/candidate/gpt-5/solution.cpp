#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  const uint8_t* const base = encoded.data();
  const uint8_t* const end = base + encoded.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const uint8_t* p = base;
    while (p < end) {
      uint32_t val;
      uint32_t b;

      b = *p++; val = (b & 0x7Fu); if ((b & 0x80u) == 0u) { sum += (uint64_t)val; continue; }
      b = *p++; val |= (uint32_t)(b & 0x7Fu) << 7; if ((b & 0x80u) == 0u) { sum += (uint64_t)val; continue; }
      b = *p++; val |= (uint32_t)(b & 0x7Fu) << 14; if ((b & 0x80u) == 0u) { sum += (uint64_t)val; continue; }
      b = *p++; val |= (uint32_t)(b & 0x7Fu) << 21; if ((b & 0x80u) == 0u) { sum += (uint64_t)val; continue; }
      b = *p++; val |= (uint32_t)(b & 0x7Fu) << 28; // fifth byte (values assumed to fit in 32 bits)
      sum += (uint64_t)val;
    }
  }

  return sum;
}
