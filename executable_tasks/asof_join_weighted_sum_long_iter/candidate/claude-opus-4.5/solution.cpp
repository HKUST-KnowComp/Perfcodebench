#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  const std::size_t probe_size = probe_ts.size();
  const std::size_t dim_size = dim_ts.size();
  
  const uint32_t* __restrict__ p_ts = probe_ts.data();
  const uint32_t* __restrict__ p_wt = probe_weights.data();
  const uint32_t* __restrict__ d_ts = dim_ts.data();
  const uint32_t* __restrict__ d_val = dim_values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t dim_idx = 0;
    
    for (std::size_t i = 0; i < probe_size; ++i) {
      const uint32_t pts = p_ts[i];
      
      // Advance dim_idx to find the largest index where dim_ts[dim_idx] <= pts
      while (dim_idx + 1 < dim_size && d_ts[dim_idx + 1] <= pts) {
        ++dim_idx;
      }
      
      sum += static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_wt[i]);
    }
  }
  return sum;
}
