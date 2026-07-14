#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  const std::size_t n = keys.size();
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t k = keys[i];
      const uint8_t  f = flags[i];
      if (f && k < key_threshold && ((k & 3u) == 0u)) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
    total = sum;   // final iteration result
  }
  return total;
}