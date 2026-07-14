#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (dim_keys.empty()) {
    return 0;
  }
  const uint32_t min_key = dim_keys.front();
  const uint32_t max_key = dim_keys.back();
  std::vector<uint32_t> lookup(max_key - min_key + 1, 0);
  const size_t n_dim = dim_keys.size();
  for (size_t i = 0; i < n_dim; ++i) {
    lookup[dim_keys[i] - min_key] = dim_values[i];
  }

  uint64_t result = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    const size_t n_probe = probe_keys.size();
    for (size_t i = 0; i < n_probe; ++i) {
      const uint32_t key = probe_keys[i];
      sum += static_cast<uint64_t>(lookup[key - min_key]) * static_cast<uint64_t>(probe_weights[i]);
    }
    result = sum;
  }
  return result;
}
