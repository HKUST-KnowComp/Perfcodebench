#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < keys.size(); ++i) {
      uint32_t key = keys[i];
      if (flags[i] && key < key_threshold && !(key & 3u)) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  return sum;
}