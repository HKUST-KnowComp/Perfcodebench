#include "interface.h"

#include <vector>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  std::vector<uint32_t> decoded;
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    decoded.clear();
    for (std::size_t i = 0; i < packed.size(); ++i) {
      decoded.push_back(static_cast<uint32_t>(packed[i] & 15u));
      decoded.push_back(static_cast<uint32_t>(packed[i] >> 4u));
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
