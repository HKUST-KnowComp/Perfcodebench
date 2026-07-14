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

  // Find the range of keys to build a direct lookup table
  uint32_t max_key = 0;
  for (uint32_t k : dim_keys) {
    if (k > max_key) max_key = k;
  }

  // Build lookup table: index is key, value is dim_value
  // Using a vector for cache-friendly contiguous memory access
  std::vector<uint32_t> lookup(max_key + 1, 0);
  for (size_t i = 0; i < dim_keys.size(); ++i) {
    lookup[dim_keys[i]] = dim_values[i];
  }

  uint64_t total_sum = 0;
  const size_t probe_size = probe_keys.size();
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_weights = probe_weights.data();
  const uint32_t* l_data = lookup.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (size_t i = 0; i < probe_size; ++i) {
      // Direct lookup is O(1)
      iter_sum += static_cast<uint64_t>(l_data[p_keys[i]]) * static_cast<uint64_t>(p_weights[i]);
    }
    total_sum = iter_sum;
  }
  return total_sum;
}