#include "interface.h"

#include <cstddef>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  // Preserve baseline behavior for iters == 0
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = keys.size();
  const uint32_t* K = keys.data();
  const uint8_t* F = flags.data();
  const uint32_t* P = payloads.data();

  // Single-pass, branchless accumulation with simple unrolling
  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
  std::size_t i = 0;
  const std::size_t limit = n & ~static_cast<std::size_t>(3);

  for (; i < limit; i += 4) {
    uint32_t k0 = K[i];
    uint32_t c0 = (F[i] != 0u) & (k0 < threshold) & ((k0 & 7u) == 0u);
    uint32_t m0 = 0u - c0;
    s0 += static_cast<uint32_t>(P[i] & m0);

    uint32_t k1 = K[i + 1];
    uint32_t c1 = (F[i + 1] != 0u) & (k1 < threshold) & ((k1 & 7u) == 0u);
    uint32_t m1 = 0u - c1;
    s1 += static_cast<uint32_t>(P[i + 1] & m1);

    uint32_t k2 = K[i + 2];
    uint32_t c2 = (F[i + 2] != 0u) & (k2 < threshold) & ((k2 & 7u) == 0u);
    uint32_t m2 = 0u - c2;
    s2 += static_cast<uint32_t>(P[i + 2] & m2);

    uint32_t k3 = K[i + 3];
    uint32_t c3 = (F[i + 3] != 0u) & (k3 < threshold) & ((k3 & 7u) == 0u);
    uint32_t m3 = 0u - c3;
    s3 += static_cast<uint32_t>(P[i + 3] & m3);
  }

  uint64_t sum = s0 + s1 + s2 + s3;

  for (; i < n; ++i) {
    uint32_t k = K[i];
    uint32_t c = (F[i] != 0u) & (k < threshold) & ((k & 7u) == 0u);
    uint32_t m = 0u - c; // 0xFFFFFFFF if selected, else 0
    sum += static_cast<uint32_t>(P[i] & m);
  }

  return sum;
}
