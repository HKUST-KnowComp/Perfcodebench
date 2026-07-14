#include <cstdint>
#include <vector>
#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

  // Edge case: empty dimension, nothing to join.
  if (dim_keys.empty() || probe_keys.empty()) {
    return 0;
  }

  uint64_t sum = 0;
  const std::size_t dim_size = dim_keys.size();
  const std::size_t probe_size = probe_keys.size();

  // Check if dimension keys form a dense, contiguous range.
  const uint32_t first_key = dim_keys.front();
  const uint32_t last_key = dim_keys.back();
  const bool dense = (static_cast<uint64_t>(last_key) - first_key + 1 == dim_size);

  if (dense) {
    // Direct lookup: key -> index = key - first_key
    const uint32_t offset = first_key;
    for (int iter = 0; iter < iters; ++iter) {
      sum = 0;
      for (std::size_t i = 0; i < probe_size; ++i) {
        const uint32_t idx = probe_keys[i] - offset;
        // idx is guaranteed to be within [0, dim_size) because the dimension
        // is dense and probe keys are valid join keys.
        sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
      }
    }
  } else {
    // Fallback: sorted dimension but not dense; use binary search (original method).
    for (int iter = 0; iter < iters; ++iter) {
      sum = 0;
      for (std::size_t i = 0; i < probe_size; ++i) {
        const auto it = std::lower_bound(dim_keys.begin(), dim_keys.end(), probe_keys[i]);
        const std::size_t idx = static_cast<std::size_t>(it - dim_keys.begin());
        sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
      }
    }
  }

  return sum;
}
