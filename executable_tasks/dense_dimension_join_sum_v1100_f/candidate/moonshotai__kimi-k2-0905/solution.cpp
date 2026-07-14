#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  // Build a direct lookup table keyed by the dimension key.
  // We only need to handle the keys that actually appear in dim_keys.
  uint32_t max_key = 0;
  for (uint32_t k : dim_keys) {
    if (k > max_key) max_key = k;
  }
  std::vector<uint32_t> lut(max_key + 1, 0);
  for (size_t i = 0; i < dim_keys.size(); ++i) {
    lut[dim_keys[i]] = dim_values[i];
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const size_t n = probe_keys.size();
    for (size_t i = 0; i < n; ++i) {
      sum += static_cast<uint64_t>(lut[probe_keys[i]]) *
             static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}