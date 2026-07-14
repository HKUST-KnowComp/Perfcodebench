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

  if (dim_keys.empty() || probe_keys.empty() || iters <= 0) {
    return 0;
  }

  // Build a direct-index lookup table for O(1) access
  // Since dimension is "dense", keys are expected to be in a reasonable range
  uint32_t max_key = 0;
  for (std::size_t i = 0; i < dim_keys.size(); ++i) {
    if (dim_keys[i] > max_key) max_key = dim_keys[i];
  }

  // Allocate lookup table: index = key, value = dim_values[corresponding index]
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
    sum = 0;
    std::size_t i = 0;

    // Unroll by 4 for better ILP and prefetching
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);
    for (; i < n4; i += 4) {
      // Prefetch ahead
      if (i + 16 < n) {
        __builtin_prefetch(&lut[pk[i + 16]], 0, 0);
      }

      uint32_t k0 = pk[i];
      uint32_t k1 = pk[i + 1];
      uint32_t k2 = pk[i + 2];
      uint32_t k3 = pk[i + 3];

      uint64_t v0 = lut[k0];
      uint64_t v1 = lut[k1];
      uint64_t v2 = lut[k2];
      uint64_t v3 = lut[k3];

      uint64_t w0 = pw[i];
      uint64_t w1 = pw[i + 1];
      uint64_t w2 = pw[i + 2];
      uint64_t w3 = pw[i + 3];

      sum += v0 * w0 + v1 * w1 + v2 * w2 + v3 * w3;
    }

    // Handle remainder
    for (; i < n; ++i) {
      sum += static_cast<uint64_t>(lut[pk[i]]) * static_cast<uint64_t>(pw[i]);
    }
  }

  return sum;
}