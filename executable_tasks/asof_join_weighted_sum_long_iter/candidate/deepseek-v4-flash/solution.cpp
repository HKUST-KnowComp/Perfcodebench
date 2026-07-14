#include "interface.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t n_probe = probe_ts.size();
  const std::size_t n_dim = dim_ts.size();

  // Precompute for each probe the index into dim_values (the latest dim_ts <= probe_ts[i])
  std::vector<std::size_t> idx_for_probe(n_probe);
  for (std::size_t i = 0; i < n_probe; ++i) {
    const auto it = std::upper_bound(dim_ts.begin(), dim_ts.end(), probe_ts[i]);
    // Assume it is not begin() – i.e., probe_ts[i] >= dim_ts[0] (as required by correctness)
    idx_for_probe[i] = static_cast<std::size_t>(it - dim_ts.begin() - 1);
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n_probe; ++i) {
      sum += static_cast<uint64_t>(dim_values[idx_for_probe[i]]) *
             static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}
