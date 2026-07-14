#include "interface.h"

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (dim_keys.empty() || probe_keys.empty()) return 0;
  const uint32_t base_key = dim_keys.front();
  const size_t num_probes = probe_keys.size();
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (size_t i = 0; i < num_probes; ++i) {
      const uint32_t pkey = probe_keys[i];
      const size_t idx = static_cast<size_t>(pkey - base_key);
      sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}