#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

  if (dim_keys.empty() || probe_keys.empty() || iters <= 0) {
    return 0;
  }

  // Find the range of dimension keys to size the lookup table
  uint32_t min_key = dim_keys[0];
  uint32_t max_key = dim_keys[0];
  for (std::size_t i = 1; i < dim_keys.size(); ++i) {
    if (dim_keys[i] < min_key) min_key = dim_keys[i];
    if (dim_keys[i] > max_key) max_key = dim_keys[i];
  }

  // Build a direct-index lookup table: lookup[key - min_key] = dim_value
  const std::size_t table_size = static_cast<std::size_t>(max_key - min_key) + 1;
  std::vector<uint32_t> lookup(table_size, 0);
  for (std::size_t i = 0; i < dim_keys.size(); ++i) {
    lookup[dim_keys[i] - min_key] = dim_values[i];
  }

  const uint32_t* __restrict__ lut = lookup.data();
  const uint32_t* __restrict__ pk = probe_keys.data();
  const uint32_t* __restrict__ pw = probe_weights.data();
  const std::size_t n = probe_keys.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;

    // Prefetch distance tuned for typical cache-line / memory latency
    constexpr std::size_t PF_DIST = 16;

    for (; i + PF_DIST < n; ++i) {
      // Prefetch future lookup location
      __builtin_prefetch(&lut[pk[i + PF_DIST] - min_key], 0, 0);
      sum += static_cast<uint64_t>(lut[pk[i] - min_key]) * static_cast<uint64_t>(pw[i]);
    }
    // Remainder without prefetch
    for (; i < n; ++i) {
      sum += static_cast<uint64_t>(lut[pk[i] - min_key]) * static_cast<uint64_t>(pw[i]);
    }
  }
  return sum;
}
