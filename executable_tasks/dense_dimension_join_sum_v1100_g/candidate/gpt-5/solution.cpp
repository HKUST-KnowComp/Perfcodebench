#include "interface.h"

#include <cstdint>
#include <cstddef>

uint64_t join_sum(
    const std::vector<uint32_t>& /*dim_keys*/,  // dense, identity-mapped
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const uint32_t* __restrict__ values = dim_values.data();
  const uint32_t* __restrict__ keys = probe_keys.data();
  const uint32_t* __restrict__ weights = probe_weights.data();
  const std::size_t n = probe_keys.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    std::size_t i = 0;

    const std::size_t n4 = n & ~static_cast<std::size_t>(3);
    for (; i < n4; i += 4) {
      const uint32_t k0 = keys[i];
      const uint32_t k1 = keys[i + 1];
      const uint32_t k2 = keys[i + 2];
      const uint32_t k3 = keys[i + 3];

      s0 += static_cast<uint64_t>(values[k0]) * static_cast<uint64_t>(weights[i]);
      s1 += static_cast<uint64_t>(values[k1]) * static_cast<uint64_t>(weights[i + 1]);
      s2 += static_cast<uint64_t>(values[k2]) * static_cast<uint64_t>(weights[i + 2]);
      s3 += static_cast<uint64_t>(values[k3]) * static_cast<uint64_t>(weights[i + 3]);
    }

    for (; i < n; ++i) {
      s0 += static_cast<uint64_t>(values[keys[i]]) * static_cast<uint64_t>(weights[i]);
    }

    sum = s0 + s1 + s2 + s3;
  }

  return sum;
}
