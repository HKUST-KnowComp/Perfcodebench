#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t m = dim_ts.size();
  const std::size_t n = probe_ts.size();

  // Precompute the dimension index for each probe timestamp using a linear scan.
  // Assumes both dim_ts and probe_ts are sorted in ascending order and that
  // for every probe_ts[i] there exists a dim_ts[t] with t <= probe_ts[i].
  std::vector<std::size_t> idx(n);
  std::size_t dim_idx = 0;
  for (std::size_t i = 0; i < n; ++i) {
    while (dim_idx + 1 < m && dim_ts[dim_idx + 1] <= probe_ts[i]) {
      ++dim_idx;
    }
    idx[i] = dim_idx;
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      sum += static_cast<uint64_t>(dim_values[idx[i]]) *
             static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}
