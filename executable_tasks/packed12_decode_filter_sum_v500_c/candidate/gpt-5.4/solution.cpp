#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const std::size_t n = packed.size();
  const uint32_t thr = static_cast<uint32_t>(threshold);
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    const uint8_t* p = packed.data();
    const uint8_t* const end = p + n;

    while (p < end) {
      const uint32_t b0 = static_cast<uint32_t>(p[0]);
      const uint32_t b1 = static_cast<uint32_t>(p[1]);
      const uint32_t b2 = static_cast<uint32_t>(p[2]);

      const uint32_t a = b0 | ((b1 & 0x0Fu) << 8u);
      const uint32_t b = (b1 >> 4u) | (b2 << 4u);

      if (a > thr) local_sum += a;
      if (b > thr) local_sum += b;

      p += 3;
    }

    sum = local_sum;
  }

  return sum;
}
