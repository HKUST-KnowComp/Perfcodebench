#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const std::size_t n = packed.size();
  const uint32_t thr = static_cast<uint32_t>(threshold);
  uint64_t sum = 0;

  const uint8_t* data = packed.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t i = 0;
    for (; i + 3u <= n; i += 3u) {
      const uint32_t b0 = static_cast<uint32_t>(data[i]);
      const uint32_t b1 = static_cast<uint32_t>(data[i + 1u]);
      const uint32_t b2 = static_cast<uint32_t>(data[i + 2u]);

      const uint32_t a = b0 | ((b1 & 0x0Fu) << 8u);
      const uint32_t b = (b1 >> 4u) | (b2 << 4u);

      if (a > thr) local_sum += static_cast<uint64_t>(a);
      if (b > thr) local_sum += static_cast<uint64_t>(b);
    }

    sum = local_sum;
  }

  return sum;
}
