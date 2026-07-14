#include "interface.h"

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  const std::size_t n = keys.size();
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      if (flags[i] && keys[i] < key_threshold && ((keys[i] & 3u) == 0u)) {
        sum += static_cast<uint64_t>(values[i]);
      }
    }
  }
  return sum;
}