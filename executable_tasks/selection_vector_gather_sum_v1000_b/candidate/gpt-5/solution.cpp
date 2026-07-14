#include "interface.h"

#include <cstddef>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  const std::size_t n = keys.size();
  const uint32_t* k = keys.data();
  const uint8_t* f = flags.data();
  const uint32_t* p = payloads.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t thr = threshold;

    std::size_t i = 0;
    const std::size_t limit = n & ~static_cast<std::size_t>(3);

    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

    for (; i < limit; i += 4) {
      uint32_t k0 = k[i];
      uint32_t k1 = k[i + 1];
      uint32_t k2 = k[i + 2];
      uint32_t k3 = k[i + 3];

      uint64_t m0 = static_cast<uint64_t>((f[i] != 0u) & (k0 < thr) & ((k0 & 7u) == 0u));
      uint64_t m1 = static_cast<uint64_t>((f[i + 1] != 0u) & (k1 < thr) & ((k1 & 7u) == 0u));
      uint64_t m2 = static_cast<uint64_t>((f[i + 2] != 0u) & (k2 < thr) & ((k2 & 7u) == 0u));
      uint64_t m3 = static_cast<uint64_t>((f[i + 3] != 0u) & (k3 < thr) & ((k3 & 7u) == 0u));

      s0 += static_cast<uint64_t>(p[i]) * m0;
      s1 += static_cast<uint64_t>(p[i + 1]) * m1;
      s2 += static_cast<uint64_t>(p[i + 2]) * m2;
      s3 += static_cast<uint64_t>(p[i + 3]) * m3;
    }

    uint64_t s = s0 + s1 + s2 + s3;

    for (; i < n; ++i) {
      uint32_t ki = k[i];
      uint64_t m = static_cast<uint64_t>((f[i] != 0u) & (ki < thr) & ((ki & 7u) == 0u));
      s += static_cast<uint64_t>(p[i]) * m;
    }

    // Match baseline behavior: return the sum from the last iteration
    sum = s;
  }

  return sum;
}
