#include "interface.h"

#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  
  const std::size_t probe_size = probe_ts.size();
  const std::size_t dim_size = dim_ts.size();
  
  if (probe_size == 0 || dim_size == 0) {
    return 0;
  }
  
  const uint32_t* __restrict__ p_ts = probe_ts.data();
  const uint32_t* __restrict__ p_wt = probe_weights.data();
  const uint32_t* __restrict__ d_ts = dim_ts.data();
  const uint32_t* __restrict__ d_val = dim_values.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t dim_idx = 0;
    
    // Find initial dimension index for first probe
    // We need the largest dim_ts[dim_idx] <= probe_ts[0]
    while (dim_idx + 1 < dim_size && d_ts[dim_idx + 1] <= p_ts[0]) {
      ++dim_idx;
    }
    
    uint32_t current_val = d_val[dim_idx];
    std::size_t i = 0;
    
    while (i < probe_size) {
      // Prefetch ahead
      if (i + 16 < probe_size) {
        __builtin_prefetch(&p_ts[i + 16], 0, 0);
        __builtin_prefetch(&p_wt[i + 16], 0, 0);
      }
      
      // Find the next dimension boundary
      uint32_t next_dim_ts = (dim_idx + 1 < dim_size) ? d_ts[dim_idx + 1] : UINT32_MAX;
      
      // Accumulate all probes that use current_val
      uint64_t local_sum = 0;
      while (i < probe_size && p_ts[i] < next_dim_ts) {
        local_sum += static_cast<uint64_t>(p_wt[i]);
        ++i;
      }
      sum += local_sum * static_cast<uint64_t>(current_val);
      
      // Advance dimension index if there are more probes
      if (i < probe_size) {
        uint32_t target = p_ts[i];
        // Linear scan with prefetch for small jumps, otherwise skip
        while (dim_idx + 1 < dim_size && d_ts[dim_idx + 1] <= target) {
          ++dim_idx;
        }
        current_val = d_val[dim_idx];
      }
    }
  }
  
  return sum;
}