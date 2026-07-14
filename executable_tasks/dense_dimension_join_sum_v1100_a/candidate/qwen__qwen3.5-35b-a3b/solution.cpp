#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  bool consecutive = true;
  uint32_t min_key = dim_keys[0];
  for (size_t i = 1; i < dim_keys.size(); ++i) {
    if (dim_keys[i] != dim_keys[i-1] + 1) {
      consecutive = false;
      break;
    }
  }

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    if (consecutive) {
      for (size_t i = 0; i < probe_keys.size(); ++i) {
        uint32_t k = probe_keys[i];
        uint32_t idx = k - min_key;
        sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
      }
    } else {
      for (size_t i = 0; i < probe_keys.size(); ++i) {
        const auto it = std::lower_bound(dim_keys.begin(), dim_keys.end(), probe_keys[i]);
        const std::size_t idx = static_cast<std::size_t>(it - dim_keys.begin());
        sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
      }
    }
  }
  return sum;
}