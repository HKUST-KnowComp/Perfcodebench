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
  if (iters <= 0 || n == 0) return 0;

  const uint32_t* __restrict k = keys.data();
  const uint32_t* __restrict v = values.data();
  const uint8_t* __restrict f = flags.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t i = 0;
    const std::size_t unroll = 8;
    const std::size_t limit = n & ~(unroll - 1);

    for (; i < limit; i += unroll) {
      uint32_t k0 = k[i + 0];
      uint32_t k1 = k[i + 1];
      uint32_t k2 = k[i + 2];
      uint32_t k3 = k[i + 3];
      uint32_t k4 = k[i + 4];
      uint32_t k5 = k[i + 5];
      uint32_t k6 = k[i + 6];
      uint32_t k7 = k[i + 7];

      if (f[i + 0] && k0 < key_threshold && ((k0 & 3u) == 0u)) local_sum += static_cast<uint64_t>(v[i + 0]);
      if (f[i + 1] && k1 < key_threshold && ((k1 & 3u) == 0u)) local_sum += static_cast<uint64_t>(v[i + 1]);
      if (f[i + 2] && k2 < key_threshold && ((k2 & 3u) == 0u)) local_sum += static_cast<uint64_t>(v[i + 2]);
      if (f[i + 3] && k3 < key_threshold && ((k3 & 3u) == 0u)) local_sum += static_cast<uint64_t>(v[i + 3]);
      if (f[i + 4] && k4 < key_threshold && ((k4 & 3u) == 0u)) local_sum += static_cast<uint64_t>(v[i + 4]);
      if (f[i + 5] && k5 < key_threshold && ((k5 & 3u) == 0u)) local_sum += static_cast<uint64_t>(v[i + 5]);
      if (f[i + 6] && k6 < key_threshold && ((k6 & 3u) == 0u)) local_sum += static_cast<uint64_t>(v[i + 6]);
      if (f[i + 7] && k7 < key_threshold && ((k7 & 3u) == 0u)) local_sum += static_cast<uint64_t>(v[i + 7]);
    }

    for (; i < n; ++i) {
      uint32_t ki = k[i];
      if (f[i] && ki < key_threshold && ((ki & 3u) == 0u)) {
        local_sum += static_cast<uint64_t>(v[i]);
      }
    }

    sum = local_sum;
  }

  return sum;
}
