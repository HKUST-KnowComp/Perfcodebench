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
      uint64_t b0 = *p++;
      if ((b0 & 0x80u) == 0) {
        local_sum += b0;
        continue;
      }

      uint64_t value = (b0 & 0x7fu);

      uint64_t b1 = *p++;
      value |= (b1 & 0x7fu) << 7;
      if ((b1 & 0x80u) == 0) {
        local_sum += value;
        continue;
      }

      uint64_t b2 = *p++;
      value |= (b2 & 0x7fu) << 14;
      if ((b2 & 0x80u) == 0) {
        local_sum += value;
        continue;
      }

      uint64_t b3 = *p++;
      value |= (b3 & 0x7fu) << 21;
      if ((b3 & 0x80u) == 0) {
        local_sum += value;
        continue;
      }

      uint64_t b4 = *p++;
      value |= (b4 & 0x7fu) << 28;
      if ((b4 & 0x80u) == 0) {
        local_sum += value;
        continue;
      }

      uint64_t b5 = *p++;
      value |= (b5 & 0x7fu) << 35;
      if ((b5 & 0x80u) == 0) {
        local_sum += value;
        continue;
      }

      uint64_t b6 = *p++;
      value |= (b6 & 0x7fu) << 42;
      if ((b6 & 0x80u) == 0) {
        local_sum += value;
        continue;
      }

      uint64_t b7 = *p++;
      value |= (b7 & 0x7fu) << 49;
      if ((b7 & 0x80u) == 0) {
        local_sum += value;
        continue;
      }

      uint64_t b8 = *p++;
      value |= (b8 & 0x7fu) << 56;
      if ((b8 & 0x80u) == 0) {
        local_sum += value;
        continue;
      }

      uint64_t b9 = *p++;
      value |= (b9 & 0x01u) << 63;
      local_sum += value;
    }

    sum = local_sum;
  }
  return sum;
}
