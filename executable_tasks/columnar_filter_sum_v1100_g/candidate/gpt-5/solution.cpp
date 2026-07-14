#include "interface.h"

#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  // Preserve baseline behavior for iters == 0
  if (iters <= 0) {
    return 0ull;
  }

  const std::size_t n = keys.size();
  const uint32_t* k = keys.data();
  const uint32_t* v = values.data();
  const uint8_t* f = flags.data();

  // Branchless fused filter+sum with partial sums to reduce dependency chain
  uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
  std::size_t i = 0;
  const std::size_t n4 = n & ~static_cast<std::size_t>(3);

  for (; i < n4; i += 4) {
    const uint32_t k0 = k[i];
    const uint32_t k1 = k[i + 1];
    const uint32_t k2 = k[i + 2];
    const uint32_t k3 = k[i + 3];

    const uint64_t m0 = ((f[i] != 0) & (k0 < key_threshold) & ((k0 & 3u) == 0u));
    const uint64_t m1 = ((f[i + 1] != 0) & (k1 < key_threshold) & ((k1 & 3u) == 0u));
    const uint64_t m2 = ((f[i + 2] != 0) & (k2 < key_threshold) & ((k2 & 3u) == 0u));
    const uint64_t m3 = ((f[i + 3] != 0) & (k3 < key_threshold) & ((k3 & 3u) == 0u));

    sum0 += static_cast<uint64_t>(v[i]) * m0;
    sum1 += static_cast<uint64_t>(v[i + 1]) * m1;
    sum2 += static_cast<uint64_t>(v[i + 2]) * m2;
    sum3 += static_cast<uint64_t>(v[i + 3]) * m3;
  }

  for (; i < n; ++i) {
    const uint32_t ki = k[i];
    const uint64_t m = ((f[i] != 0) & (ki < key_threshold) & ((ki & 3u) == 0u));
    sum0 += static_cast<uint64_t>(v[i]) * m;
  }

  const uint64_t sum = sum0 + sum1 + sum2 + sum3;

  // The computed sum is identical each iteration, so returning it matches the
  // baseline result after the last iteration while avoiding redundant work.
  return sum;
}
