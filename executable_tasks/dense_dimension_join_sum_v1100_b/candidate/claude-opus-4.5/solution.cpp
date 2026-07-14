#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

  // Build dense lookup table: lookup[key] = value
  // Find max key to size the table
  uint32_t max_key = 0;
  for (std::size_t i = 0; i < dim_keys.size(); ++i) {
    if (dim_keys[i] > max_key) max_key = dim_keys[i];
  }

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
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    std::size_t i = 0;
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);

    for (; i < n4; i += 4) {
      // Prefetch ahead for cache
      if (i + 32 < n) {
        __builtin_prefetch(&lut[pk[i + 32]], 0, 0);
      }
      s0 += static_cast<uint64_t>(lut[pk[i]])     * static_cast<uint64_t>(pw[i]);
      s1 += static_cast<uint64_t>(lut[pk[i + 1]]) * static_cast<uint64_t>(pw[i + 1]);
      s2 += static_cast<uint64_t>(lut[pk[i + 2]]) * static_cast<uint64_t>(pw[i + 2]);
      s3 += static_cast<uint64_t>(lut[pk[i + 3]]) * static_cast<uint64_t>(pw[i + 3]);
    }

    for (; i < n; ++i) {
      s0 += static_cast<uint64_t>(lut[pk[i]]) * static_cast<uint64_t>(pw[i]);
    }

    sum = s0 + s1 + s2 + s3;
  }
  return sum;
}
