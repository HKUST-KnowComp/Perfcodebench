#include "interface.h"

#include <vector>
#include <cstdint>
#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

  // Determine the largest key so we know how much to allocate.
  uint32_t max_key = 0;
  if (!dim_keys.empty()) {
    max_key = *std::max_element(dim_keys.begin(), dim_keys.end());
  }

  // Build a direct lookup table: key -> value.
  std::vector<uint64_t> lut(max_key + 1);
  for (size_t i = 0; i < dim_keys.size(); ++i) {
    lut[dim_keys[i]] = dim_values[i];
  }

  uint64_t sum = 0;
  const size_t n = probe_keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      local_sum += lut[probe_keys[i]] * static_cast<uint64_t>(probe_weights[i]);
    }
    sum = local_sum;
  }
  return sum;
}