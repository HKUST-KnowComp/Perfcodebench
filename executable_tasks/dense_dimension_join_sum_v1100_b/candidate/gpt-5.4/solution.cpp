#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t dim_n = dim_keys.size();
  const std::size_t probe_n = probe_keys.size();

  if (iters <= 0 || probe_n == 0 || dim_n == 0) {
    return 0;
  }

  // Dense dimension optimization:
  // Build a direct lookup table from key -> value once, then probe in O(1).
  // The task guarantees a dense dimension, so this is much faster than
  // repeated binary searches.
  uint32_t max_key = 0;
  for (std::size_t i = 0; i < dim_n; ++i) {
    if (dim_keys[i] > max_key) max_key = dim_keys[i];
  }

  std::vector<uint32_t> lookup(static_cast<std::size_t>(max_key) + 1);
  for (std::size_t i = 0; i < dim_n; ++i) {
    lookup[dim_keys[i]] = dim_values[i];
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t i = 0;
    const std::size_t unroll_end = probe_n & ~std::size_t(3);
    for (; i < unroll_end; i += 4) {
      const uint32_t k0 = probe_keys[i];
      const uint32_t k1 = probe_keys[i + 1];
      const uint32_t k2 = probe_keys[i + 2];
      const uint32_t k3 = probe_keys[i + 3];

      local_sum += static_cast<uint64_t>(lookup[k0]) * static_cast<uint64_t>(probe_weights[i]);
      local_sum += static_cast<uint64_t>(lookup[k1]) * static_cast<uint64_t>(probe_weights[i + 1]);
      local_sum += static_cast<uint64_t>(lookup[k2]) * static_cast<uint64_t>(probe_weights[i + 2]);
      local_sum += static_cast<uint64_t>(lookup[k3]) * static_cast<uint64_t>(probe_weights[i + 3]);
    }
    for (; i < probe_n; ++i) {
      local_sum += static_cast<uint64_t>(lookup[probe_keys[i]]) * static_cast<uint64_t>(probe_weights[i]);
    }

    sum = local_sum;
  }
  return sum;
}
