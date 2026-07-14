#include "interface.h"

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (iters <= 0 || dim_keys.empty() || probe_keys.empty() || probe_weights.size() != probe_keys.size()) {
    return 0;
  }

  const uint32_t first_key = dim_keys[0];
  const uint32_t last_key = dim_keys.back();
  const size_t num_probes = probe_keys.size();
  uint64_t final_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (size_t i = 0; i < num_probes; ++i) {
      const uint32_t k = probe_keys[i];
      if (k >= first_key && k <= last_key) {
        const size_t idx = k - first_key;
        current_sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
      }
    }
    final_sum = current_sum;
  }

  return final_sum;
}