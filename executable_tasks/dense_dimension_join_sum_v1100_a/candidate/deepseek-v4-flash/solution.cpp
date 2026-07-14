#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (dim_keys.empty()) return 0;

  uint32_t min_key = dim_keys[0];
  uint32_t max_key = dim_keys[0];
  for (auto k : dim_keys) {
    if (k < min_key) min_key = k;
    if (k > max_key) max_key = k;
  }

  std::size_t range_size = static_cast<std::size_t>(max_key - min_key) + 1;
  std::vector<uint32_t> direct_lookup(range_size);
  for (std::size_t i = 0; i < dim_keys.size(); ++i) {
    direct_lookup[dim_keys[i] - min_key] = dim_values[i];
  }

  uint64_t sum = 0;
  const std::size_t n = probe_keys.size();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      std::size_t idx = static_cast<std::size_t>(probe_keys[i] - min_key);
      sum += static_cast<uint64_t>(direct_lookup[idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}
