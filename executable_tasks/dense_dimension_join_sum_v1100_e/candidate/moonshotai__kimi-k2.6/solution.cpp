#include "interface.h"

#include <algorithm>
#include <cstddef>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (iters <= 0 || probe_keys.empty()) {
    return 0;
  }

  const std::size_t n = probe_keys.size();
  uint64_t sum = 0;

  // Dense 0-based: keys are exactly [0, N)
  if (!dim_keys.empty() && dim_keys.front() == 0 && dim_keys.back() == dim_keys.size() - 1) {
    for (std::size_t i = 0; i < n; ++i) {
      sum += static_cast<uint64_t>(dim_values[probe_keys[i]]) * static_cast<uint64_t>(probe_weights[i]);
    }
    return sum;
  }

  // Dense with constant offset: keys are [K, K+N)
  if (!dim_keys.empty() && dim_keys.back() - dim_keys.front() == dim_keys.size() - 1) {
    const uint32_t min_key = dim_keys.front();
    for (std::size_t i = 0; i < n; ++i) {
      sum += static_cast<uint64_t>(dim_values[probe_keys[i] - min_key]) * static_cast<uint64_t>(probe_weights[i]);
    }
    return sum;
  }

  // Sparse fallback: binary search
  for (std::size_t i = 0; i < n; ++i) {
    const auto it = std::lower_bound(dim_keys.begin(), dim_keys.end(), probe_keys[i]);
    const std::size_t idx = static_cast<std::size_t>(it - dim_keys.begin());
    sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
  }
  return sum;
}
