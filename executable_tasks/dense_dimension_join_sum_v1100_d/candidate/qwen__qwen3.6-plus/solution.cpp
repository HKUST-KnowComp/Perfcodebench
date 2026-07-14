#include "interface.h"

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (probe_keys.empty() || iters <= 0) return 0;

  const uint32_t offset = dim_keys.empty() ? 0 : dim_keys[0];
  const uint32_t* dim_vals = dim_values.data();
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_weights = probe_weights.data();
  const size_t n = probe_keys.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current = 0;
    for (size_t i = 0; i < n; ++i) {
      current += static_cast<uint64_t>(dim_vals[p_keys[i] - offset]) * p_weights[i];
    }
    sum = current;
  }
  return sum;
}