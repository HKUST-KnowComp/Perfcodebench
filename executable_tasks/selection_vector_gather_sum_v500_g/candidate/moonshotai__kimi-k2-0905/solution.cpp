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
  const uint32_t* k = keys.data();
  const uint8_t*  f = flags.data();
  const uint32_t* p = payloads.data();

  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;

    // Unroll 4× to amortize loop overhead and give the compiler more ILP.
    std::size_t i = 0;
    for (; i + 3 < n; i += 4) {
      const uint32_t k0 = k[i];
      const uint32_t k1 = k[i + 1];
      const uint32_t k2 = k[i + 2];
      const uint32_t k3 = k[i + 3];

      const uint8_t  m0 = f[i];
      const uint8_t  m1 = f[i + 1];
      const uint8_t  m2 = f[i + 2];
      const uint8_t  m3 = f[i + 3];

      const uint32_t t0 = (k0 < threshold) & ((k0 & 7u) == 0u) & (m0 != 0u);
      const uint32_t t1 = (k1 < threshold) & ((k1 & 7u) == 0u) & (m1 != 0u);
      const uint32_t t2 = (k2 < threshold) & ((k2 & 7u) == 0u) & (m2 != 0u);
      const uint32_t t3 = (k3 < threshold) & ((k3 & 7u) == 0u) & (m3 != 0u);

      sum += static_cast<uint64_t>(p[i    ]) * t0;
      sum += static_cast<uint64_t>(p[i + 1]) * t1;
      sum += static_cast<uint64_t>(p[i + 2]) * t2;
      sum += static_cast<uint64_t>(p[i + 3]) * t3;
    }

    // Tail
    for (; i < n; ++i) {
      const uint32_t ki = k[i];
      const uint8_t  fi = f[i];
      const uint32_t mask = (ki < threshold) & ((ki & 7u) == 0u) & (fi != 0u);
      sum += static_cast<uint64_t>(p[i]) * mask;
    }

    total = sum;  // store final value for return
  }

  return total;
}