#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (dim_keys.empty() || probe_keys.empty()) {
    return 0;
  }
  // Precompute dense lookup table once before any iterations
  const uint32_t min_key = dim_keys.front();
  const uint32_t max_key = dim_keys.back();
  std::vector<uint32_t> value_lookup(max_key - min_key + 1);
  for (size_t i = 0; i < dim_keys.size(); ++i) {
    value_lookup[dim_keys[i] - min_key] = dim_values[i];
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (size_t i = 0; i < probe_keys.size(); ++i) {
      const uint32_t dim_val = value_lookup[probe_keys[i] - min_key];
      sum += static_cast<uint64_t>(dim_val) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}