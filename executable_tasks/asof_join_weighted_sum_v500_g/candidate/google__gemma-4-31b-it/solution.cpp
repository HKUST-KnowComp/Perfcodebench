#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const size_t dim_size = dim_ts.size();
  const size_t probe_size = probe_ts.size();
  const uint32_t* d_ts = dim_ts.data();
  const uint32_t* d_val = dim_values.data();
  const uint32_t* p_ts = probe_ts.data();
  const uint32_t* p_w = probe_weights.data();

  uint64_t total_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t d_idx = 0;
    
    for (size_t p_idx = 0; p_idx < probe_size; ++p_idx) {
      uint32_t target = p_ts[p_idx];
      // Advance d_idx to the last element <= target
      while (d_idx < dim_size && d_ts[d_idx] <= target) {
        d_idx++;
      }
      
      // The as-of value is at d_idx - 1
      if (d_idx > 0) {
        current_sum += static_cast<uint64_t>(d_val[d_idx - 1]) * static_cast<uint64_t>(p_w[p_idx]);
      }
    }
    total_sum = current_sum;
  }
  return total_sum;
}