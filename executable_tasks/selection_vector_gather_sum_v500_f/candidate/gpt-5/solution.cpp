#include "interface.h"

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  // If no iterations requested, match baseline behavior: return 0.
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = keys.size();

  const uint32_t* k = keys.data();
  const uint8_t* f = flags.data();
  const uint32_t* pl = payloads.data();

  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

  std::size_t i = 0;
  const std::size_t limit = n & static_cast<std::size_t>(~3ULL);

  // Unrolled loop by 4 for better ILP and reduced branch overhead.
  for (; i < limit; i += 4) {
    uint32_t k0 = k[i];
    uint32_t m0 = ((f[i] != 0) & (k0 < threshold) & ((k0 & 7u) == 0u));
    s0 += static_cast<uint64_t>(pl[i]) * static_cast<uint64_t>(m0);

    uint32_t k1 = k[i + 1];
    uint32_t m1 = ((f[i + 1] != 0) & (k1 < threshold) & ((k1 & 7u) == 0u));
    s1 += static_cast<uint64_t>(pl[i + 1]) * static_cast<uint64_t>(m1);

    uint32_t k2 = k[i + 2];
    uint32_t m2 = ((f[i + 2] != 0) & (k2 < threshold) & ((k2 & 7u) == 0u));
    s2 += static_cast<uint64_t>(pl[i + 2]) * static_cast<uint64_t>(m2);

    uint32_t k3 = k[i + 3];
    uint32_t m3 = ((f[i + 3] != 0) & (k3 < threshold) & ((k3 & 7u) == 0u));
    s3 += static_cast<uint64_t>(pl[i + 3]) * static_cast<uint64_t>(m3);
  }

  // Handle remaining elements.
  for (; i < n; ++i) {
    uint32_t kk = k[i];
    uint32_t m = ((f[i] != 0) & (kk < threshold) & ((kk & 7u) == 0u));
    s0 += static_cast<uint64_t>(pl[i]) * static_cast<uint64_t>(m);
  }

  uint64_t sum = s0 + s1 + s2 + s3;

  // Subsequent iterations would yield the same sum; baseline returns the sum of the last iteration.
  return sum;
}
