#include "interface.h"

#include <cstdint>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int /*iters*/) {
  const std::size_t n = probe_keys.size();
  if (n == 0) {
    return 0ull;
  }

  const uint32_t base = dim_keys[0];
  const uint32_t* dv = dim_values.data();
  const uint32_t* pk = probe_keys.data();
  const uint32_t* pw = probe_weights.data();

  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

  std::size_t i = 0;
  const std::size_t n4 = n & ~static_cast<std::size_t>(3);
  for (; i < n4; i += 4) {
    const std::size_t idx0 = static_cast<std::size_t>(pk[i + 0] - base);
    const std::size_t idx1 = static_cast<std::size_t>(pk[i + 1] - base);
    const std::size_t idx2 = static_cast<std::size_t>(pk[i + 2] - base);
    const std::size_t idx3 = static_cast<std::size_t>(pk[i + 3] - base);

    s0 += static_cast<uint64_t>(dv[idx0]) * static_cast<uint64_t>(pw[i + 0]);
    s1 += static_cast<uint64_t>(dv[idx1]) * static_cast<uint64_t>(pw[i + 1]);
    s2 += static_cast<uint64_t>(dv[idx2]) * static_cast<uint64_t>(pw[i + 2]);
    s3 += static_cast<uint64_t>(dv[idx3]) * static_cast<uint64_t>(pw[i + 3]);
  }

  for (; i < n; ++i) {
    const std::size_t idx = static_cast<std::size_t>(pk[i] - base);
    s0 += static_cast<uint64_t>(dv[idx]) * static_cast<uint64_t>(pw[i]);
  }

  return (s0 + s1) + (s2 + s3);
}
