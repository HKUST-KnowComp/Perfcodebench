#include "interface.h"

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  const std::size_t n = keys.size();
  const uint32_t* __restrict__ key_data = keys.data();
  const uint32_t* __restrict__ val_data = values.data();
  const uint8_t* __restrict__ flag_data = flags.data();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      if (flag_data[i] && key_data[i] < key_threshold && ((key_data[i] & 3u) == 0u)) {
        sum += static_cast<uint64_t>(val_data[i]);
      }
    }
  }
  return sum;
}