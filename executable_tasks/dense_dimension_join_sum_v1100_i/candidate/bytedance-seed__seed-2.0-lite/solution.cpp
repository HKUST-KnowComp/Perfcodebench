#include "interface.h"

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  if (dim_keys.empty() || probe_keys.empty()) {
    return sum;
  }
  // Precompute base key once for dense dimension indexing
  const uint32_t min_dim_key = dim_keys[0];
  const std::size_t probe_size = probe_keys.size();
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (std::size_t i = 0; i < probe_size; ++i) {
      const uint32_t p = probe_keys[i];
      const std::size_t idx = static_cast<std::size_t>(p - min_dim_key);
      iter_sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
    sum = iter_sum;
  }
  return sum;
}