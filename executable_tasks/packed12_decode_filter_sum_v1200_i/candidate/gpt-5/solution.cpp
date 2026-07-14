#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  if (iters <= 0) {
    return 0;
  }

  const std::size_t ntriplets = packed.size() / 3u;
  const uint8_t* p = packed.data();
  const uint32_t thr = static_cast<uint32_t>(threshold);
  uint64_t sum = 0;

  for (std::size_t t = 0; t < ntriplets; ++t) {
    const uint32_t b0 = static_cast<uint32_t>(p[0]);
    const uint32_t b1 = static_cast<uint32_t>(p[1]);
    const uint32_t b2 = static_cast<uint32_t>(p[2]);

    const uint32_t a = b0 | ((b1 & 0x0Fu) << 8);
    const uint32_t b = (b1 >> 4) | (b2 << 4);

    if (a > thr) sum += a;
    if (b > thr) sum += b;

    p += 3;
  }

  return sum;
}
