#include "interface.h"

#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  // Handle empty dimension gracefully
  if (dim_keys.empty()) {
    return 0;
  }

  const std::size_t dim_size = dim_keys.size();
  const uint32_t min_key = dim_keys[0];
  const uint32_t max_key = dim_keys[dim_size - 1];
  const uint64_t range = static_cast<uint64_t>(max_key) - min_key + 1;

  // Precompute dense lookup if the keys form a contiguous range (dense dimension)
  std::vector<uint32_t> lookup;
  bool use_direct = (range == dim_size);
  if (use_direct) {
    lookup.resize(range);
    for (std::size_t i = 0; i < dim_size; ++i) {
      lookup[dim_keys[i] - min_key] = dim_values[i];
    }
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    if (use_direct) {
      for (std::size_t i = 0; i < probe_keys.size(); ++i) {
        const uint32_t key = probe_keys[i];
        // Keys are assumed to lie in [min_key, max_key] (true for inner join)
        const uint32_t val = lookup[key - min_key];
        sum += static_cast<uint64_t>(val) * static_cast<uint64_t>(probe_weights[i]);
      }
    } else {
      // Fallback: original binary search join (correct for any sorted dimension)
      for (std::size_t i = 0; i < probe_keys.size(); ++i) {
        const auto it = std::lower_bound(dim_keys.begin(), dim_keys.end(), probe_keys[i]);
        const std::size_t idx = static_cast<std::size_t>(it - dim_keys.begin());
        sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
      }
    }
  }
  return sum;
}