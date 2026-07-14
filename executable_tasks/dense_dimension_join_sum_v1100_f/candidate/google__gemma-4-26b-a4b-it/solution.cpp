#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (dim_keys.empty()) return 0;

  // Find the range of keys to build a direct lookup table.
  // Given the task title 'dense dimension join', we assume keys are relatively compact.
  uint32_t max_key = 0;
  for (uint32_t k : dim_keys) {
    if (k > max_key) max_key = k;
  }

  // Create a lookup table for O(1) access.
  // We use a vector to handle the mapping. 
  // If max_key is extremely large, a hash map would be better, but for 'dense' dimensions, 
  // a direct array is the fastest possible approach.
  std::vector<uint32_t> lookup(max_key + 1, 0);
  for (std::size_t i = 0; i < dim_keys.size(); ++i) {
    lookup[dim_keys[i]] = dim_values[i];
  }

  uint64_t total_sum = 0;
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_weights = probe_weights.data();
  const std::size_t n = probe_keys.size();
  const uint32_t* l_ptr = lookup.data();
  const uint32_t max_k = max_key;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t k = p_keys[i];
      // Safety check: only access if key is within the dimension range.
      // In a dense join context, probe keys are expected to be valid.
      if (k <= max_k) {
        current_iter_sum += static_cast<uint64_t>(l_ptr[k]) * p_weights[i];
      }
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}