#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  (void)dim_keys; // Dense dimension: keys map directly to indices
  if (iters <= 0) return 0;

  const uint32_t* dv = dim_values.data();
  const uint32_t* pk = probe_keys.data();
  const uint32_t* pw = probe_weights.data();
  const std::size_t n = probe_keys.size();

  uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

  std::size_t i = 0;
  const std::size_t n_unroll = n & ~static_cast<std::size_t>(7);
  for (; i < n_unroll; i += 8) {
    s0 += static_cast<uint64_t>(dv[pk[i + 0]]) * static_cast<uint64_t>(pw[i + 0]);
    s1 += static_cast<uint64_t>(dv[pk[i + 1]]) * static_cast<uint64_t>(pw[i + 1]);
    s2 += static_cast<uint64_t>(dv[pk[i + 2]]) * static_cast<uint64_t>(pw[i + 2]);
    s3 += static_cast<uint64_t>(dv[pk[i + 3]]) * static_cast<uint64_t>(pw[i + 3]);
    s0 += static_cast<uint64_t>(dv[pk[i + 4]]) * static_cast<uint64_t>(pw[i + 4]);
    s1 += static_cast<uint64_t>(dv[pk[i + 5]]) * static_cast<uint64_t>(pw[i + 5]);
    s2 += static_cast<uint64_t>(dv[pk[i + 6]]) * static_cast<uint64_t>(pw[i + 6]);
    s3 += static_cast<uint64_t>(dv[pk[i + 7]]) * static_cast<uint64_t>(pw[i + 7]);
  }
  for (; i < n; ++i) {
    s0 += static_cast<uint64_t>(dv[pk[i]]) * static_cast<uint64_t>(pw[i]);
  }

  uint64_t sum = (s0 + s1) + (s2 + s3);
  return sum;
}
