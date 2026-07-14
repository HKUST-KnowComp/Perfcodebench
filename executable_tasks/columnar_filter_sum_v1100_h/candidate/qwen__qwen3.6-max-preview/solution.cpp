#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  uint64_t sum = 0;
  const std::size_t n = keys.size();
  const uint32_t* k = keys.data();
  const uint32_t* v = values.data();
  const uint8_t* f = flags.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      if (f[i] && k[i] < key_threshold && (k[i] & 3u) == 0u) {
        current_sum += v[i];
      }
    }
    sum = current_sum;
  }
  return sum;
}