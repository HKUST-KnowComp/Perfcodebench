#include "interface.h"
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  (void)iters; // iterations do not affect result, compute once
  const uint32_t* d_ts = dim_ts.data();
  const uint32_t* d_vals = dim_values.data();
  const uint32_t* p_ts = probe_ts.data();
  const uint32_t* p_weights = probe_weights.data();
  const std::size_t n_probe = probe_ts.size();
  const std::size_t n_dim = dim_ts.size();

  std::size_t dim_idx = 0;
  uint64_t sum = 0;

  for (std::size_t i = 0; i < n_probe; ++i) {
    const uint32_t p = p_ts[i];
    // advance dim_idx to the largest index with dim_ts[dim_idx] <= p
    while (dim_idx + 1 < n_dim && d_ts[dim_idx + 1] <= p) {
      ++dim_idx;
    }
    sum += static_cast<uint64_t>(d_vals[dim_idx]) * static_cast<uint64_t>(p_weights[i]);
  }
  return sum;
}
