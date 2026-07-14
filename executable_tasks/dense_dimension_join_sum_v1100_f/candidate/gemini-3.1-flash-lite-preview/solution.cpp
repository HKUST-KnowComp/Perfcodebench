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

  // Precompute a lookup table for O(1) access.
  // Assuming dim_keys are dense and within a reasonable range.
  uint32_t min_key = dim_keys.front();
  uint32_t max_key = dim_keys.back();
  uint32_t range = max_key - min_key + 1;

  std::vector<uint32_t> lookup(range);
  for (size_t i = 0; i < dim_keys.size(); ++i) {
    lookup[dim_keys[i] - min_key] = dim_values[i];
  }

  uint64_t total_sum = 0;
  const size_t n = probe_keys.size();
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_weights = probe_weights.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      // Direct lookup is significantly faster than binary search
      uint32_t key = p_keys[i];
      if (key >= min_key && key <= max_key) {
        iter_sum += static_cast<uint64_t>(lookup[key - min_key]) * static_cast<uint64_t>(p_weights[i]);
      }
    }
    total_sum = iter_sum;
  }
  return total_sum;
}