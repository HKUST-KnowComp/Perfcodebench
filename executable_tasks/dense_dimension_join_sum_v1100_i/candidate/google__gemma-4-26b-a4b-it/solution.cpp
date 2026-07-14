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

  // Since the dimension is 'dense', we can optimize the lookup.
  // We first find the range of keys to see if a direct lookup table is feasible.
  uint32_t min_key = dim_keys[0];
  uint32_t max_key = dim_keys[0];
  for (uint32_t k : dim_keys) {
    if (k < min_key) min_key = k;
    if (k > max_key) max_key = k;
  }

  uint64_t total_sum = 0;
  size_t n_probes = probe_keys.size();

  // If the key range is reasonably small, use a direct lookup table (O(1) probe).
  // Otherwise, fallback to binary search or a hash map. 
  // For 'dense dimension' tasks, the range is typically manageable.
  uint32_t range = max_key - min_key + 1;
  
  if (range < 10000000) { // 10M entries ~ 40MB
    std::vector<uint32_t> lookup(range, 0);
    // Note: The baseline used lower_bound which implies dim_keys is sorted.
    // If dim_keys is sorted, we can map keys to values directly.
    for (size_t i = 0; i < dim_keys.size(); ++i) {
      lookup[dim_keys[i] - min_key] = dim_values[i];
    }

    for (int iter = 0; iter < iters; ++iter) {
      uint64_t current_sum = 0;
      for (size_t i = 0; i < n_probes; ++i) {
        uint32_t k = probe_keys[i];
        // Check bounds to ensure correctness if probe_keys contains keys outside dim range
        if (k >= min_key && k <= max_key) {
          current_sum += static_cast<uint64_t>(lookup[k - min_key]) * probe_weights[i];
        } else {
          // Fallback for keys outside the dense range if necessary
          // However, in a 'dense dimension join', keys are expected to exist.
          // The baseline used lower_bound which might return an index that doesn't match the key.
          // To match baseline exactly: 
          auto it = std::lower_bound(dim_keys.begin(), dim_keys.end(), k);
          if (it != dim_keys.end() && *it == k) {
             current_sum += static_cast<uint64_t>(dim_values[std::distance(dim_keys.begin(), it)]) * probe_weights[i];
          }
        }
      }
      total_sum = current_sum;
    }
  } else {
    // Fallback to optimized binary search if range is too large
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t current_sum = 0;
      for (size_t i = 0; i < n_probes; ++i) {
        const auto it = std::lower_bound(dim_keys.begin(), dim_keys.end(), probe_keys[i]);
        const std::size_t idx = static_cast<std::size_t>(it - dim_keys.begin());
        current_sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
      }
      total_sum = current_sum;
    }
  }

  return total_sum;
}