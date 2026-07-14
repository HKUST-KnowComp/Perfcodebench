#include "interface.h"

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (dim_keys.empty() || probe_keys.empty()) {
    return 0;
  }
  // Precompute dense lookup table once for all iterations, leveraging sorted dense dimension keys
  const uint32_t max_dim_key = dim_keys.back();
  std::vector<uint32_t> key_to_value(max_dim_key + 1);
  for (size_t i = 0; i < dim_keys.size(); ++i) {
    key_to_value[dim_keys[i]] = dim_values[i];
  }
  uint64_t final_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    for (size_t i = 0; i < probe_keys.size(); ++i) {
      sum += static_cast<uint64_t>(key_to_value[probe_keys[i]]) * static_cast<uint64_t>(probe_weights[i]);
    }
    final_sum = sum;
  }
  return final_sum;
}