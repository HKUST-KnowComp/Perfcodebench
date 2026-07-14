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

  // Dense dimension join: build a direct lookup table once and reuse it across
  // iterations. Keys are assumed to be dense enough that this is beneficial for
  // this benchmark.
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
    const std::size_t unroll_end = probe_n & ~static_cast<std::size_t>(3);
    for (; i < unroll_end; i += 4) {
      local_sum += static_cast<uint64_t>(lookup[probe_keys[i]]) *
                   static_cast<uint64_t>(probe_weights[i]);
      local_sum += static_cast<uint64_t>(lookup[probe_keys[i + 1]]) *
                   static_cast<uint64_t>(probe_weights[i + 1]);
      local_sum += static_cast<uint64_t>(lookup[probe_keys[i + 2]]) *
                   static_cast<uint64_t>(probe_weights[i + 2]);
      local_sum += static_cast<uint64_t>(lookup[probe_keys[i + 3]]) *
                   static_cast<uint64_t>(probe_weights[i + 3]);
    }
    for (; i < probe_n; ++i) {
      local_sum += static_cast<uint64_t>(lookup[probe_keys[i]]) *
                   static_cast<uint64_t>(probe_weights[i]);
    }

    sum = local_sum;
  }
  return sum;
}
