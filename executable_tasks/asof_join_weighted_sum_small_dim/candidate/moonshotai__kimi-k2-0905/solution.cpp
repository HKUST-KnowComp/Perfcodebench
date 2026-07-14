#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (dim_ts.empty()) return 0;

  const std::size_t n_dim = dim_ts.size();
  const std::size_t n_probe = probe_ts.size();

  const uint32_t* __restrict d_ts   = dim_ts.data();
  const uint32_t* __restrict d_vals = dim_values.data();
  const uint32_t* __restrict p_ts   = probe_ts.data();
  const uint32_t* __restrict p_ws   = probe_weights.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t dim_idx = 0;
    for (std::size_t probe_idx = 0; probe_idx < n_probe; ++probe_idx) {
      const uint32_t t_probe = p_ts[probe_idx];
      while (dim_idx + 1 < n_dim && d_ts[dim_idx + 1] <= t_probe) {
        ++dim_idx;
      }
      sum += static_cast<uint64_t>(d_vals[dim_idx]) * static_cast<uint64_t>(p_ws[probe_idx]);
    }
  }

  return sum;
}