#include "interface.h"

#include <algorithm>
#include <cassert>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  assert(std::is_sorted(dim_keys.begin(), dim_keys.end()));
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      const auto it = std::lower_bound(dim_keys.begin(), dim_keys.end(), probe_keys[i]);
      if (it != dim_keys.end() && *it == probe_keys[i]) {
        const std::size_t idx = static_cast<std::size_t>(it - dim_keys.begin());
        sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
      }
    }
  }
  return sum;
}