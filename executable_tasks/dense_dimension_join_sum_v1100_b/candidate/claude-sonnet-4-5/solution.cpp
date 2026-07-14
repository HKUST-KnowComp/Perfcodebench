#include "interface.h"

#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  
  // Build direct lookup array for dense dimension
  uint32_t max_key = dim_keys.empty() ? 0 : dim_keys.back();
  std::vector<uint32_t> lookup(max_key + 1);
  
  for (std::size_t i = 0; i < dim_keys.size(); ++i) {
    lookup[dim_keys[i]] = dim_values[i];
  }
  
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      sum += static_cast<uint64_t>(lookup[probe_keys[i]]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}