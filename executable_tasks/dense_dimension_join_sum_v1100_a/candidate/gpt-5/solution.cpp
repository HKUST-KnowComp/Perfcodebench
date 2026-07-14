#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  // Handle trivial cases quickly
  if (dim_values.empty() || probe_keys.empty()) {
    return 0ULL;
  }

  const uint32_t base = dim_keys.empty() ? 0u : dim_keys.front();
  const uint32_t* __restrict values = dim_values.data();
  const uint32_t* __restrict keys = probe_keys.data();
  const uint32_t* __restrict weights = probe_weights.data();
  const std::size_t n = probe_keys.size();

  uint64_t sum = 0ULL;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    std::size_t i = 0;

    // Unrolled loop in chunks of 4 for better ILP
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);
    for (; i < n4; i += 4) {
      const std::size_t idx0 = static_cast<std::size_t>(keys[i + 0] - base);
      const std::size_t idx1 = static_cast<std::size_t>(keys[i + 1] - base);
      const std::size_t idx2 = static_cast<std::size_t>(keys[i + 2] - base);
      const std::size_t idx3 = static_cast<std::size_t>(keys[i + 3] - base);

      s0 += static_cast<uint64_t>(values[idx0]) * static_cast<uint64_t>(weights[i + 0]);
      s1 += static_cast<uint64_t>(values[idx1]) * static_cast<uint64_t>(weights[i + 1]);
      s2 += static_cast<uint64_t>(values[idx2]) * static_cast<uint64_t>(weights[i + 2]);
      s3 += static_cast<uint64_t>(values[idx3]) * static_cast<uint64_t>(weights[i + 3]);
    }

    // Remainder
    for (; i < n; ++i) {
      const std::size_t idx = static_cast<std::size_t>(keys[i] - base);
      s0 += static_cast<uint64_t>(values[idx]) * static_cast<uint64_t>(weights[i]);
    }

    sum = (s0 + s1) + (s2 + s3);
  }

  return sum;
}
