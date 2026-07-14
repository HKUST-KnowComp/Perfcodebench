#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  const std::size_t n = keys.size();
  const uint32_t* __restrict k = keys.data();
  const uint8_t* __restrict f = flags.data();
  const uint32_t* __restrict p = payloads.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    std::size_t i = 0;
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);
    for (; i < n4; i += 4) {
      uint32_t k0 = k[i], k1 = k[i+1], k2 = k[i+2], k3 = k[i+3];
      uint8_t f0 = f[i], f1 = f[i+1], f2 = f[i+2], f3 = f[i+3];
      if (f0 && k0 < threshold && ((k0 & 7u) == 0u)) s0 += p[i];
      if (f1 && k1 < threshold && ((k1 & 7u) == 0u)) s1 += p[i+1];
      if (f2 && k2 < threshold && ((k2 & 7u) == 0u)) s2 += p[i+2];
      if (f3 && k3 < threshold && ((k3 & 7u) == 0u)) s3 += p[i+3];
    }
    for (; i < n; ++i) {
      if (f[i] && k[i] < threshold && ((k[i] & 7u) == 0u)) {
        s0 += p[i];
      }
    }
    sum = s0 + s1 + s2 + s3;
  }
  return sum;
}
