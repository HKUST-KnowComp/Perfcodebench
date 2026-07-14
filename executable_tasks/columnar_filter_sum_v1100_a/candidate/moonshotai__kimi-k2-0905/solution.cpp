#include "interface.h"

#include <cstdint>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  uint64_t sum = 0;
  const size_t n = keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      const uint32_t k = keys[i];
      if (flags[i] && k < key_threshold && ((k & 3u) == 0u)) {
        local_sum += values[i];
      }
    }
    sum = local_sum;
  }

  return sum;
}