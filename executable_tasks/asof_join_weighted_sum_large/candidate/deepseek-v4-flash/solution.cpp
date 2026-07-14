#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  // Precompute the index in dim_ts for each probe timestamp.
  std::vector<std::size_t> idx(probe_ts.size());
  std::size_t dim_idx = 0;
  const std::size_t n_dim = dim_ts.size();
  const std::size_t n_probe = probe_ts.size();
  for (std::size_t i = 0; i < n_probe; ++i) {
    while (dim_idx < n_dim && dim_ts[dim_idx] <= probe_ts[i]) {
      ++dim_idx;
    }
    // dim_idx is the first strictly greater than probe_ts[i], so the match is one before.
    idx[i] = dim_idx - 1;
  }

  // Accumulate over iterations.
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n_probe; ++i) {
      sum += static_cast<uint64_t>(dim_values[idx[i]]) *
             static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}
