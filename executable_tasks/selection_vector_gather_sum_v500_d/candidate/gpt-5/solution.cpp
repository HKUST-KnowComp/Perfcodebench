#include "interface.h"

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  const std::size_t n = keys.size();
  if (n == 0 || iters <= 0) {
    return 0ull;
  }

  const uint32_t* k = keys.data();
  const uint8_t* f = flags.data();
  const uint32_t* p = payloads.data();

  uint64_t sum = 0ull;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0ull;
    uint64_t s1 = 0ull;

    std::size_t i = 0;
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);
    for (; i < n4; i += 4) {
      uint32_t k0 = k[i];
      uint32_t k1 = k[i + 1];
      uint32_t k2 = k[i + 2];
      uint32_t k3 = k[i + 3];

      uint64_t m0 = (static_cast<uint64_t>(f[i] != 0) & static_cast<uint64_t>(k0 < threshold) & static_cast<uint64_t>((k0 & 7u) == 0u));
      uint64_t m1 = (static_cast<uint64_t>(f[i + 1] != 0) & static_cast<uint64_t>(k1 < threshold) & static_cast<uint64_t>((k1 & 7u) == 0u));
      uint64_t m2 = (static_cast<uint64_t>(f[i + 2] != 0) & static_cast<uint64_t>(k2 < threshold) & static_cast<uint64_t>((k2 & 7u) == 0u));
      uint64_t m3 = (static_cast<uint64_t>(f[i + 3] != 0) & static_cast<uint64_t>(k3 < threshold) & static_cast<uint64_t>((k3 & 7u) == 0u));

      s0 += static_cast<uint64_t>(p[i]) * m0;
      s1 += static_cast<uint64_t>(p[i + 1]) * m1;
      s0 += static_cast<uint64_t>(p[i + 2]) * m2;
      s1 += static_cast<uint64_t>(p[i + 3]) * m3;
    }

    uint64_t s = s0 + s1;

    for (; i < n; ++i) {
      uint32_t ki = k[i];
      uint64_t m = (static_cast<uint64_t>(f[i] != 0) & static_cast<uint64_t>(ki < threshold) & static_cast<uint64_t>((ki & 7u) == 0u));
      s += static_cast<uint64_t>(p[i]) * m;
    }

    sum = s;
  }

  return sum;
}
