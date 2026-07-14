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

  // Build direct-indexed lookup table for O(1) access
  // Find max key to size the table
  uint32_t max_key = 0;
  const std::size_t dim_size = dim_keys.size();
  for (std::size_t i = 0; i < dim_size; ++i) {
    if (dim_keys[i] > max_key) max_key = dim_keys[i];
  }

  // Create lookup table: index = key, value = dim_values[corresponding index]
  std::vector<uint32_t> lookup(static_cast<std::size_t>(max_key) + 1, 0);
  for (std::size_t i = 0; i < dim_size; ++i) {
    lookup[dim_keys[i]] = dim_values[i];
  }

  const uint32_t* __restrict__ lut = lookup.data();
  const uint32_t* __restrict__ pkeys = probe_keys.data();
  const uint32_t* __restrict__ pweights = probe_weights.data();
  const std::size_t probe_size = probe_keys.size();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    std::size_t i = 0;
    const std::size_t unroll_limit = probe_size & ~static_cast<std::size_t>(3);

    // Prefetch ahead
    for (; i < unroll_limit; i += 4) {
      // Prefetch future keys
      if (i + 16 < probe_size) {
        __builtin_prefetch(&lut[pkeys[i + 16]], 0, 1);
        __builtin_prefetch(&lut[pkeys[i + 17]], 0, 1);
        __builtin_prefetch(&lut[pkeys[i + 18]], 0, 1);
        __builtin_prefetch(&lut[pkeys[i + 19]], 0, 1);
      }

      const uint32_t k0 = pkeys[i];
      const uint32_t k1 = pkeys[i + 1];
      const uint32_t k2 = pkeys[i + 2];
      const uint32_t k3 = pkeys[i + 3];

      const uint32_t v0 = lut[k0];
      const uint32_t v1 = lut[k1];
      const uint32_t v2 = lut[k2];
      const uint32_t v3 = lut[k3];

      const uint32_t w0 = pweights[i];
      const uint32_t w1 = pweights[i + 1];
      const uint32_t w2 = pweights[i + 2];
      const uint32_t w3 = pweights[i + 3];

      s0 += static_cast<uint64_t>(v0) * static_cast<uint64_t>(w0);
      s1 += static_cast<uint64_t>(v1) * static_cast<uint64_t>(w1);
      s2 += static_cast<uint64_t>(v2) * static_cast<uint64_t>(w2);
      s3 += static_cast<uint64_t>(v3) * static_cast<uint64_t>(w3);
    }

    // Handle remainder
    for (; i < probe_size; ++i) {
      s0 += static_cast<uint64_t>(lut[pkeys[i]]) * static_cast<uint64_t>(pweights[i]);
    }

    sum = s0 + s1 + s2 + s3;
  }

  return sum;
}
