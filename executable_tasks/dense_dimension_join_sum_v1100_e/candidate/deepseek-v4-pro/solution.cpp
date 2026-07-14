#include "interface.h"

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t N = probe_keys.size();
  const std::size_t D = dim_keys.size();
  if (N == 0 || D == 0) return 0;
  const uint32_t min_key = dim_keys[0];
  const uint32_t* dim_vals = dim_values.data();
  const uint32_t* pkeys = probe_keys.data();
  const uint32_t* pw = probe_weights.data();
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    for (std::size_t i = 0; i < N; ++i) {
      const std::size_t idx = static_cast<std::size_t>(pkeys[i] - min_key);
      local_sum += static_cast<uint64_t>(dim_vals[idx]) * static_cast<uint64_t>(pw[i]);
    }
    sum = local_sum;
  }
  return sum;
}