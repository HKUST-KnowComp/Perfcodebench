#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  const uint8_t* const data = encoded.data();
  const std::size_t size = encoded.size();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = data;
    const uint8_t* const end = data + size;
    uint64_t local_sum = 0;

    while (p < end) {
      uint8_t b = *p++;
      if ((b & 0x80u) == 0) {
        local_sum += static_cast<uint64_t>(b);
        continue;
      }

      uint32_t value = static_cast<uint32_t>(b & 0x7Fu);
      uint32_t shift = 7;
      do {
        b = *p++;
        value |= static_cast<uint32_t>(b & 0x7Fu) << shift;
        shift += 7;
      } while (b & 0x80u);

      local_sum += static_cast<uint64_t>(value);
    }

    sum = local_sum;
  }

  return sum;
}
