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

  const uint32_t* k = keys.data();
  const uint32_t* v = values.data();
  const uint8_t* f = flags.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0;
    uint64_t s1 = 0;
    uint64_t s2 = 0;
    uint64_t s3 = 0;

    std::size_t i = 0;
    const std::size_t unroll_end = n & ~std::size_t(3);
    for (; i < unroll_end; i += 4) {
      const uint32_t k0 = k[i + 0];
      const uint32_t k1 = k[i + 1];
      const uint32_t k2 = k[i + 2];
      const uint32_t k3 = k[i + 3];

      if (f[i + 0] && k0 < key_threshold && ((k0 & 3u) == 0u)) s0 += static_cast<uint64_t>(v[i + 0]);
      if (f[i + 1] && k1 < key_threshold && ((k1 & 3u) == 0u)) s1 += static_cast<uint64_t>(v[i + 1]);
      if (f[i + 2] && k2 < key_threshold && ((k2 & 3u) == 0u)) s2 += static_cast<uint64_t>(v[i + 2]);
      if (f[i + 3] && k3 < key_threshold && ((k3 & 3u) == 0u)) s3 += static_cast<uint64_t>(v[i + 3]);
    }

    for (; i < n; ++i) {
      const uint32_t ki = k[i];
      if (f[i] && ki < key_threshold && ((ki & 3u) == 0u)) {
        s0 += static_cast<uint64_t>(v[i]);
      }
    }

    sum = s0 + s1 + s2 + s3;
  }
  return sum;
}
