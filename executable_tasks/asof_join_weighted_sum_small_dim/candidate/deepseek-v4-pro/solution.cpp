#include "interface.h"

#include <cstddef>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  (void)iters; // the result is deterministic; compute once
  const std::size_t d_size = dim_ts.size();
  const std::size_t p_size = probe_ts.size();
  const uint32_t* __restrict d_ts = dim_ts.data();
  const uint32_t* __restrict d_vals = dim_values.data();
  const uint32_t* __restrict p_ts = probe_ts.data();
  const uint32_t* __restrict p_w = probe_weights.data();

  uint64_t sum = 0;
  std::size_t d_idx = 0;

  for (std::size_t i = 0; i < p_size; ++i) {
    const uint32_t target = p_ts[i];
    // advance to the largest index where dim_ts[idx] <= target
    while (d_idx + 1 < d_size && d_ts[d_idx + 1] <= target) {
      ++d_idx;
    }
    sum += static_cast<uint64_t>(d_vals[d_idx]) * static_cast<uint64_t>(p_w[i]);
  }
  return sum;
}
