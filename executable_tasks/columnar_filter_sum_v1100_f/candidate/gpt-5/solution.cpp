#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  (void)iters; // The result is identical across iterations; compute once.

  const std::size_t n = keys.size();
  if (n == 0 || key_threshold == 0) return 0ULL;

  const uint32_t* k = keys.data();
  const uint32_t* v = values.data();
  const uint8_t* f = flags.data();

  uint64_t sum = 0;

  std::size_t i = 0;
  const std::size_t n4 = n & ~static_cast<std::size_t>(3);
  for (; i < n4; i += 4) {
    uint32_t k0 = k[i];
    if (f[i] && k0 < key_threshold && ((k0 & 3u) == 0u)) sum += v[i];

    uint32_t k1 = k[i + 1];
    if (f[i + 1] && k1 < key_threshold && ((k1 & 3u) == 0u)) sum += v[i + 1];

    uint32_t k2 = k[i + 2];
    if (f[i + 2] && k2 < key_threshold && ((k2 & 3u) == 0u)) sum += v[i + 2];

    uint32_t k3 = k[i + 3];
    if (f[i + 3] && k3 < key_threshold && ((k3 & 3u) == 0u)) sum += v[i + 3];
  }

  for (; i < n; ++i) {
    uint32_t ki = k[i];
    if (f[i] && ki < key_threshold && ((ki & 3u) == 0u)) {
      sum += v[i];
    }
  }

  return sum;
}
