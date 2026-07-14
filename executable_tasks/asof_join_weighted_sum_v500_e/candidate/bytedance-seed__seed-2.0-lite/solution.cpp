// Optimized asof_join_sum using sorted-input two-pointer scan
#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const uint32_t* d_ts = dim_ts.data();
    const uint32_t* d_val = dim_values.data();
    const size_t d_len = dim_ts.size();
    const uint32_t* p_ts = probe_ts.data();
    const uint32_t* p_w = probe_weights.data();
    const size_t p_len = probe_ts.size();
    
    size_t d_idx = 0;
    for (size_t i = 0; i < p_len; ++i) {
      const uint32_t current_probe_ts = p_ts[i];
      // Advance dimension pointer only forward, leveraging sorted timestamps
      while (d_idx + 1 < d_len && d_ts[d_idx + 1] <= current_probe_ts) {
        d_idx++;
      }
      sum += static_cast<uint64_t>(d_val[d_idx]) * static_cast<uint64_t>(p_w[i]);
    }
  }
  return sum;
}