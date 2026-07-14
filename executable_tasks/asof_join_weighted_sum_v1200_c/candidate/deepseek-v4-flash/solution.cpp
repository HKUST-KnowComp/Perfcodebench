#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t n = probe_ts.size();
  const std::size_t m = dim_ts.size();

  // Precompute as-of join index for each probe timestamp using a single pass
  std::vector<std::size_t> idx(n);
  std::size_t ptr = 0;
  for (std::size_t i = 0; i < n; ++i) {
    while (ptr + 1 < m && dim_ts[ptr + 1] <= probe_ts[i]) {
      ++ptr;
    }
    idx[i] = ptr;
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      sum += static_cast<uint64_t>(dim_values[idx[i]]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}