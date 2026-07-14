#include "interface.h"

#include <vector>

uint64_t packed12_filter_sum(const std::vector<uint8_t>& packed, uint16_t threshold, int iters) {
  std::vector<uint32_t> decoded;
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    decoded.clear();
    decoded.reserve((packed.size() / 3u) * 2u);
    for (std::size_t i = 0; i < packed.size(); i += 3u) {
      const uint32_t a =
          static_cast<uint32_t>(packed[i]) |
          (static_cast<uint32_t>(packed[i + 1u] & 0x0fu) << 8u);
      const uint32_t b =
          (static_cast<uint32_t>(packed[i + 1u] >> 4u)) |
          (static_cast<uint32_t>(packed[i + 2u]) << 4u);
      decoded.push_back(a);
      decoded.push_back(b);
    }
    sum = 0;
    for (uint32_t v : decoded) {
      if (v > static_cast<uint32_t>(threshold)) {
        sum += static_cast<uint64_t>(v);
      }
    }
  }
  return sum;
}
