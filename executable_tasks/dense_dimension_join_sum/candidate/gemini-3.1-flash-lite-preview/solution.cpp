#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (dim_keys.empty()) return 0;

  uint32_t max_key = 0;
  for (uint32_t k : dim_keys) if (k > max_key) max_key = k;

  std::vector<uint32_t> lookup(max_key + 1, 0);
  for (size_t i = 0; i < dim_keys.size(); ++i) {
    lookup[dim_keys[i]] = dim_values[i];
  }

  uint64_t total_sum = 0;
  const size_t probe_size = probe_keys.size();
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_weights = probe_weights.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (size_t i = 0; i < probe_size; ++i) {
      uint32_t key = p_keys[i];
      if (key <= max_key) {
        iter_sum += static_cast<uint64_t>(lookup[key]) * static_cast<uint64_t>(p_weights[i]);
      }
    }
    total_sum = iter_sum;
  }
  return total_sum;
}