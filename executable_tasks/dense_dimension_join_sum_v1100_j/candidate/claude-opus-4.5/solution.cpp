#include "interface.h"

#include <algorithm>
#include <cstdint>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

  if (dim_keys.empty() || probe_keys.empty() || iters <= 0) {
    return 0;
  }

  // Find max key to size the lookup table
  uint32_t max_key = 0;
  for (std::size_t i = 0; i < dim_keys.size(); ++i) {
    if (dim_keys[i] > max_key) max_key = dim_keys[i];
  }

  // Build dense lookup table: lookup[key] = value
  std::vector<uint32_t> lookup(static_cast<std::size_t>(max_key) + 1, 0);
  for (std::size_t i = 0; i < dim_keys.size(); ++i) {
    lookup[dim_keys[i]] = dim_values[i];
  }

  const uint32_t* __restrict__ lut = lookup.data();
  const uint32_t* __restrict__ pk = probe_keys.data();
  const uint32_t* __restrict__ pw = probe_weights.data();
  const std::size_t n = probe_keys.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    for (std::size_t i = 0; i < n; ++i) {
      s += static_cast<uint64_t>(lut[pk[i]]) * static_cast<uint64_t>(pw[i]);
    }
    sum = s;
  }
  return sum;
}
