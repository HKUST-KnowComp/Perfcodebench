#include "interface.h"

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const uint32_t thr = static_cast<uint32_t>(threshold);
  // Fast path: 12-bit values are in [0, 4095]. If threshold >= 4095, nothing can exceed it.
  if (thr >= 0x0FFFu) {
    return 0u;
  }

  const uint8_t* data = packed.data();
  const std::size_t n3 = (packed.size() / 3u) * 3u; // number of bytes in complete 3-byte groups

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    const uint8_t* p = data;
    const uint8_t* end = data + n3;
    while (p < end) {
      const uint32_t b0 = static_cast<uint32_t>(p[0]);
      const uint32_t b1 = static_cast<uint32_t>(p[1]);
      const uint32_t b2 = static_cast<uint32_t>(p[2]);

      const uint32_t a = b0 | ((b1 & 0x0Fu) << 8);
      const uint32_t b = (b1 >> 4) | (b2 << 4);

      if (a > thr) s += a;
      if (b > thr) s += b;

      p += 3;
    }
    sum = s;
  }
  return sum;
}
