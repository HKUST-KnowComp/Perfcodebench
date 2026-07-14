#include "interface.h"

#include <cstdint>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  // Decode once and accumulate directly into sum.
  // The sum is identical across all iterations, so the outer loop is redundant.
  uint64_t sum = 0;
  const uint8_t* ptr = encoded.data();
  const uint8_t* end = ptr + encoded.size();
  while (ptr < end) {
    uint32_t value = 0;
    uint32_t shift = 0;
    uint8_t byte;
    do {
      byte = *ptr++;
      value |= static_cast<uint32_t>(byte & 0x7F) << shift;
      shift += 7;
    } while (byte & 0x80);
    sum += value;
  }
  return sum;
}
