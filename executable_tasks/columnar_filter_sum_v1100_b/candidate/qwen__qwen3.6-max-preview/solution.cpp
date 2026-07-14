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
  const size_t n = keys.size();
  const uint32_t* k = keys.data();
  const uint32_t* v = values.data();
  const uint8_t* f = flags.data();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (size_t i = 0; i < n; ++i) {
      uint32_t ki = k[i];
      if (f[i] && ki < key_threshold && (ki & 3u) == 0u) {
        sum += v[i];
      }
    }
  }
  return sum;
}
