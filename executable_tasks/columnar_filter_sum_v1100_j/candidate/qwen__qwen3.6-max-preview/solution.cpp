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
  (void)iters;
  if (keys.empty()) return 0;

  uint64_t sum = 0;
  const uint32_t* __restrict__ k = keys.data();
  const uint32_t* __restrict__ v = values.data();
  const uint8_t* __restrict__ f = flags.data();
  const size_t n = keys.size();

  for (size_t i = 0; i < n; ++i) {
    if (f[i] && k[i] < key_threshold && (k[i] & 3u) == 0u) {
      sum += v[i];
    }
  }
  return sum;
}