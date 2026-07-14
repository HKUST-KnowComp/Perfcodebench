#include "interface.h"

#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const size_t n3 = packed.size();
  const size_t num_values = (n3 / 3) * 2;
  std::vector<uint32_t> decoded(num_values);
  
  // Decode all packed values into decoded vector (once)
  const uint8_t* src = packed.data();
  uint32_t* dst = decoded.data();
  const uint8_t* end = src + n3;
  while (src < end) {
    uint32_t byte0 = src[0];
    uint32_t byte1 = src[1];
    uint32_t byte2 = src[2];
    *dst++ = byte0 | ((byte1 & 0x0F) << 8);
    *dst++ = (byte1 >> 4) | (byte2 << 4);
    src += 3;
  }
  
  uint64_t final_sum = 0;
  uint32_t th = threshold;
  const uint32_t* values = decoded.data();
  const uint32_t* values_end = values + num_values;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    const uint32_t* p = values;
    while (p < values_end) {
      if (*p > th) s += *p;
      ++p;
    }
    final_sum = s;  // keep last iteration's sum
  }
  
  return final_sum;
}
