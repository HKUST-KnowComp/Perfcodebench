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
  if (iters <= 0 || n == 0) return 0;

  const uint32_t* k = keys.data();
  const uint8_t* f = flags.data();
  const uint32_t* p = payloads.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t i = 0;
    const std::size_t unroll_end = n & ~std::size_t(7);
    for (; i < unroll_end; i += 8) {
      uint32_t k0 = k[i + 0];
      uint32_t k1 = k[i + 1];
      uint32_t k2 = k[i + 2];
      uint32_t k3 = k[i + 3];
      uint32_t k4 = k[i + 4];
      uint32_t k5 = k[i + 5];
      uint32_t k6 = k[i + 6];
      uint32_t k7 = k[i + 7];

      if (f[i + 0] && k0 < threshold && ((k0 & 7u) == 0u)) local_sum += static_cast<uint64_t>(p[i + 0]);
      if (f[i + 1] && k1 < threshold && ((k1 & 7u) == 0u)) local_sum += static_cast<uint64_t>(p[i + 1]);
      if (f[i + 2] && k2 < threshold && ((k2 & 7u) == 0u)) local_sum += static_cast<uint64_t>(p[i + 2]);
      if (f[i + 3] && k3 < threshold && ((k3 & 7u) == 0u)) local_sum += static_cast<uint64_t>(p[i + 3]);
      if (f[i + 4] && k4 < threshold && ((k4 & 7u) == 0u)) local_sum += static_cast<uint64_t>(p[i + 4]);
      if (f[i + 5] && k5 < threshold && ((k5 & 7u) == 0u)) local_sum += static_cast<uint64_t>(p[i + 5]);
      if (f[i + 6] && k6 < threshold && ((k6 & 7u) == 0u)) local_sum += static_cast<uint64_t>(p[i + 6]);
      if (f[i + 7] && k7 < threshold && ((k7 & 7u) == 0u)) local_sum += static_cast<uint64_t>(p[i + 7]);
    }

    for (; i < n; ++i) {
      const uint32_t key = k[i];
      if (f[i] && key < threshold && ((key & 7u) == 0u)) {
        local_sum += static_cast<uint64_t>(p[i]);
      }
    }

    sum = local_sum;
  }
  return sum;
}
