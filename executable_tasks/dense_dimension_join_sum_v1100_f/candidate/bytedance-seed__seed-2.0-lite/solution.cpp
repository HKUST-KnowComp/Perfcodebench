#include "interface.h"

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (dim_keys.empty() || probe_keys.empty()) return 0;

  // Precompute single cache-resident lookup table once for all iterations
  const uint32_t min_dim_key = dim_keys[0];
  const uint32_t max_dim_key = dim_keys.back();
  std::vector<uint32_t> key_to_value(max_dim_key - min_dim_key + 1);
  
  for (size_t i = 0; i < dim_keys.size(); ++i) {
    key_to_value[dim_keys[i] - min_dim_key] = dim_values[i];
  }

  uint64_t sum = 0;
  const size_t probe_size = probe_keys.size();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (size_t i = 0; i < probe_size; ++i) {
      const uint32_t pkey = probe_keys[i];
      const uint32_t dim_val = key_to_value[pkey - min_dim_key];
      sum += static_cast<uint64_t>(dim_val) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}