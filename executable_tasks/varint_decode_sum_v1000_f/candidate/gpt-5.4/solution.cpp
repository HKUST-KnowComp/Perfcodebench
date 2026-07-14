#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  const uint8_t* const data = encoded.data();
  const std::size_t n = encoded.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* p = data;
    const uint8_t* const end = data + n;
    uint64_t local_sum = 0;

    while (p < end) {
      uint32_t b0 = *p++;
      if ((b0 & 0x80u) == 0u) {
        local_sum += b0;
        continue;
      }

      uint32_t value = b0 & 0x7fu;

      uint32_t b1 = *p++;
      value |= (b1 & 0x7fu) << 7;
      if ((b1 & 0x80u) == 0u) {
        local_sum += value;
        continue;
      }

      uint32_t b2 = *p++;
      value |= (b2 & 0x7fu) << 14;
      if ((b2 & 0x80u) == 0u) {
        local_sum += value;
        continue;
      }

      uint32_t b3 = *p++;
      value |= (b3 & 0x7fu) << 21;
      if ((b3 & 0x80u) == 0u) {
        local_sum += value;
        continue;
      }

      uint32_t b4 = *p++;
      value |= (b4 & 0x7fu) << 28;
      local_sum += value;
    }

    sum = local_sum;
  }
  return sum;
}
