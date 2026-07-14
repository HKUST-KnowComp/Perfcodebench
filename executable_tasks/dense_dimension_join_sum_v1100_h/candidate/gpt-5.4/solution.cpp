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

  // Dense dimension join: build a direct lookup table once, then probe by key.
  // This replaces per-probe binary search with O(1) indexed loads.
  uint32_t max_key = 0;
  for (std::size_t i = 0; i < dim_n; ++i) {
    const uint32_t k = dim_keys[i];
    if (k > max_key) max_key = k;
  }

  std::vector<uint32_t> lookup(static_cast<std::size_t>(max_key) + 1);
  for (std::size_t i = 0; i < dim_n; ++i) {
    lookup[dim_keys[i]] = dim_values[i];
  }

  const uint32_t* lookup_ptr = lookup.data();
  const uint32_t* probe_k = probe_keys.data();
  const uint32_t* probe_w = probe_weights.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t i = 0;
    const std::size_t unroll_end = probe_n & ~static_cast<std::size_t>(3);
    for (; i < unroll_end; i += 4) {
      local_sum += static_cast<uint64_t>(lookup_ptr[probe_k[i]]) * static_cast<uint64_t>(probe_w[i]);
      local_sum += static_cast<uint64_t>(lookup_ptr[probe_k[i + 1]]) * static_cast<uint64_t>(probe_w[i + 1]);
      local_sum += static_cast<uint64_t>(lookup_ptr[probe_k[i + 2]]) * static_cast<uint64_t>(probe_w[i + 2]);
      local_sum += static_cast<uint64_t>(lookup_ptr[probe_k[i + 3]]) * static_cast<uint64_t>(probe_w[i + 3]);
    }
    for (; i < probe_n; ++i) {
      local_sum += static_cast<uint64_t>(lookup_ptr[probe_k[i]]) * static_cast<uint64_t>(probe_w[i]);
    }

    sum = local_sum;
  }

  return sum;
}
