#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  
  // Find the range of keys to determine the size of the lookup table
  uint32_t min_key = dim_keys[0];
  uint32_t max_key = dim_keys[0];
  for (uint32_t k : dim_keys) {
    if (k < min_key) min_key = k;
    if (k > max_key) max_key = k;
  }

  // Create a direct-mapped lookup table
  std::vector<uint32_t> lookup(max_key - min_key + 1);
  for (size_t i = 0; i < dim_keys.size(); ++i) {
    lookup[dim_keys[i] - min_key] = dim_values[i];
  }

  uint64_t total_sum = 0;
  const size_t probe_size = probe_keys.size();
  const uint32_t* pk_ptr = probe_keys.data();
  const uint32_t* pw_ptr = probe_weights.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < probe_size; ++i) {
      // Direct lookup is O(1)
      uint32_t val = lookup[pk_ptr[i] - min_key];
      current_iter_sum += static_cast<uint64_t>(val) * static_cast<uint64_t>(pw_ptr[i]);
    }
    total_sum = current_iter_sum;
  }
  
  return total_sum;
}