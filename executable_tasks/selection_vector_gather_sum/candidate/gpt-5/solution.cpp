#include "interface.h"

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  if (iters <= 0) return 0;

  std::size_t n = keys.size();
  if (flags.size() < n) n = flags.size();
  if (payloads.size() < n) n = payloads.size();

  const uint32_t* k = keys.data();
  const uint8_t* f = flags.data();
  const uint32_t* p = payloads.data();

  uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;

  std::size_t i = 0;
  std::size_t limit = n & ~static_cast<std::size_t>(3);
  for (; i < limit; i += 4) {
    uint32_t k0 = k[i];
    uint32_t k1 = k[i + 1];
    uint32_t k2 = k[i + 2];
    uint32_t k3 = k[i + 3];

    uint64_t m0 = static_cast<uint64_t>( (f[i] != 0) && (k0 < threshold) && ((k0 & 7u) == 0u) );
    uint64_t m1 = static_cast<uint64_t>( (f[i + 1] != 0) && (k1 < threshold) && ((k1 & 7u) == 0u) );
    uint64_t m2 = static_cast<uint64_t>( (f[i + 2] != 0) && (k2 < threshold) && ((k2 & 7u) == 0u) );
    uint64_t m3 = static_cast<uint64_t>( (f[i + 3] != 0) && (k3 < threshold) && ((k3 & 7u) == 0u) );

    sum0 += static_cast<uint64_t>(p[i]) * m0;
    sum1 += static_cast<uint64_t>(p[i + 1]) * m1;
    sum2 += static_cast<uint64_t>(p[i + 2]) * m2;
    sum3 += static_cast<uint64_t>(p[i + 3]) * m3;
  }
  for (; i < n; ++i) {
    uint32_t ki = k[i];
    uint64_t m = static_cast<uint64_t>( (f[i] != 0) && (ki < threshold) && ((ki & 7u) == 0u) );
    sum0 += static_cast<uint64_t>(p[i]) * m;
  }

  return sum0 + sum1 + sum2 + sum3;
}
