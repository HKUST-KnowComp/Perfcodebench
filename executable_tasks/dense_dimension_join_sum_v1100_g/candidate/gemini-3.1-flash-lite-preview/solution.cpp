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

  uint32_t min_key = dim_keys.front();
  uint32_t max_key = dim_keys.back();
  uint32_t range = max_key - min_key + 1;

  std::vector<uint32_t> lookup(range);
  for (size_t i = 0; i < dim_keys.size(); ++i) {
    lookup[dim_keys[i] - min_key] = dim_values[i];
  }

  uint64_t total_sum = 0;
  const size_t probe_size = probe_keys.size();
  const uint32_t* pk = probe_keys.data();
  const uint32_t* pw = probe_weights.data();
  const uint32_t* lu = lookup.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (size_t i = 0; i < probe_size; ++i) {
      uint32_t key = pk[i];
      if (key >= min_key && key <= max_key) {
        iter_sum += static_cast<uint64_t>(lu[key - min_key]) * static_cast<uint64_t>(pw[i]);
      }
    }
    total_sum = iter_sum;
  }
  return total_sum;
}