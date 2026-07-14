#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const size_t dim_size = dim_ts.size();
  const size_t probe_size = probe_ts.size();
  
  if (dim_size == 0 || probe_size == 0) return 0;

  const uint32_t* d_ts = dim_ts.data();
  const uint32_t* d_val = dim_values.data();
  const uint32_t* p_ts = probe_ts.data();
  const uint32_t* p_w = probe_weights.data();

  uint64_t total_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t dim_idx = 0;
    
    for (size_t i = 0; i < probe_size; ++i) {
      uint32_t target = p_ts[i];
      // Move dim_idx to the last element <= target
      while (dim_idx + 1 < dim_size && d_ts[dim_idx + 1] <= target) {
        dim_idx++;
      }
      // The as-of join logic: if target < dim_ts[0], this is technically undefined 
      // based on the baseline (it would access index -1). 
      // However, the baseline uses upper_bound - 1. 
      // If target < dim_ts[0], upper_bound returns begin(), idx becomes -1.
      // To preserve baseline behavior exactly, we check if target >= d_ts[0].
      if (target >= d_ts[0]) {
        current_sum += static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_w[i]);
      }
    }
    total_sum = current_sum;
  }
  return total_sum;
}