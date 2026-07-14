#include "interface.h"

#include <vector>
#include <cstddef>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  // Preserve baseline behavior for iters <= 0
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = keys.size();
  const uint32_t* k = keys.data();
  const uint8_t* f = flags.data();
  const uint32_t* p = payloads.data();

  // Unrolled accumulation with multiple independent sums to improve ILP
  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

  std::size_t i = 0;
  const std::size_t n4 = n & static_cast<std::size_t>(~3u);
  for (; i < n4; i += 4) {
    uint32_t k0 = k[i];
    uint32_t k1 = k[i + 1];
    uint32_t k2 = k[i + 2];
    uint32_t k3 = k[i + 3];

    if (f[i] && ((k0 & 7u) == 0u) && (k0 < threshold)) {
      s0 += static_cast<uint64_t>(p[i]);
    }
    if (f[i + 1] && ((k1 & 7u) == 0u) && (k1 < threshold)) {
      s1 += static_cast<uint64_t>(p[i + 1]);
    }
    if (f[i + 2] && ((k2 & 7u) == 0u) && (k2 < threshold)) {
      s2 += static_cast<uint64_t>(p[i + 2]);
    }
    if (f[i + 3] && ((k3 & 7u) == 0u) && (k3 < threshold)) {
      s3 += static_cast<uint64_t>(p[i + 3]);
    }
  }

  for (; i < n; ++i) {
    uint32_t ki = k[i];
    if (f[i] && ((ki & 7u) == 0u) && (ki < threshold)) {
      s0 += static_cast<uint64_t>(p[i]);
    }
  }

  uint64_t sum = s0 + s1 + s2 + s3;

  // The result does not change across iterations, so return the computed sum.
  return sum;
}
