#include "interface.h"

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const uint32_t base_key = dim_keys[0];
  uint64_t sum = 0;
  const std::size_t num_probes = probe_keys.size();
  const uint32_t* dim_vals = dim_values.data();
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_weights = probe_weights.data();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < num_probes; ++i) {
      const std::size_t idx = static_cast<std::size_t>(p_keys[i] - base_key);
      sum += static_cast<uint64_t>(dim_vals[idx]) * static_cast<uint64_t>(p_weights[i]);
    }
  }
  return sum;
}