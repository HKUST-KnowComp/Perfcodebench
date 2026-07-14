#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  const std::size_t n = packed.size();
  const uint32_t thr = static_cast<uint32_t>(threshold);
  const uint8_t* data = packed.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t i = 0;
    const std::size_t unrolled_end = n - (n % 12u);

    for (; i < unrolled_end; i += 12u) {
      uint32_t a0 = static_cast<uint32_t>(data[i + 0]) |
                    (static_cast<uint32_t>(data[i + 1] & 0x0Fu) << 8u);
      uint32_t b0 = (static_cast<uint32_t>(data[i + 1]) >> 4u) |
                    (static_cast<uint32_t>(data[i + 2]) << 4u);

      uint32_t a1 = static_cast<uint32_t>(data[i + 3]) |
                    (static_cast<uint32_t>(data[i + 4] & 0x0Fu) << 8u);
      uint32_t b1 = (static_cast<uint32_t>(data[i + 4]) >> 4u) |
                    (static_cast<uint32_t>(data[i + 5]) << 4u);

      uint32_t a2 = static_cast<uint32_t>(data[i + 6]) |
                    (static_cast<uint32_t>(data[i + 7] & 0x0Fu) << 8u);
      uint32_t b2 = (static_cast<uint32_t>(data[i + 7]) >> 4u) |
                    (static_cast<uint32_t>(data[i + 8]) << 4u);

      uint32_t a3 = static_cast<uint32_t>(data[i + 9]) |
                    (static_cast<uint32_t>(data[i + 10] & 0x0Fu) << 8u);
      uint32_t b3 = (static_cast<uint32_t>(data[i + 10]) >> 4u) |
                    (static_cast<uint32_t>(data[i + 11]) << 4u);

      if (a0 > thr) local_sum += a0;
      if (b0 > thr) local_sum += b0;
      if (a1 > thr) local_sum += a1;
      if (b1 > thr) local_sum += b1;
      if (a2 > thr) local_sum += a2;
      if (b2 > thr) local_sum += b2;
      if (a3 > thr) local_sum += a3;
      if (b3 > thr) local_sum += b3;
    }

    for (; i < n; i += 3u) {
      const uint32_t a = static_cast<uint32_t>(data[i]) |
                         (static_cast<uint32_t>(data[i + 1] & 0x0Fu) << 8u);
      const uint32_t b = (static_cast<uint32_t>(data[i + 1]) >> 4u) |
                         (static_cast<uint32_t>(data[i + 2]) << 4u);
      if (a > thr) local_sum += a;
      if (b > thr) local_sum += b;
    }

    sum = local_sum;
  }

  return sum;
}
