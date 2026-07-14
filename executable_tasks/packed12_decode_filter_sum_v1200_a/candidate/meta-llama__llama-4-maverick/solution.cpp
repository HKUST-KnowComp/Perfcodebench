#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  uint64_t sum = 0;
  const std::size_t count = (packed.size() / 3u) * 2u;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < packed.size(); i += 3u) {
      const uint32_t a =
          static_cast<uint32_t>(packed[i]) |
          (static_cast<uint32_t>(packed[i + 1u] & 0x0fu) << 8u);
      const uint32_t b =
          (static_cast<uint32_t>(packed[i + 1u] >> 4u)) |
          (static_cast<uint32_t>(packed[i + 2u]) << 4u);
      sum += (a > threshold) ? static_cast<uint64_t>(a) : 0;
      sum += (b > threshold) ? static_cast<uint64_t>(b) : 0;
    }
  }
  return sum;
}