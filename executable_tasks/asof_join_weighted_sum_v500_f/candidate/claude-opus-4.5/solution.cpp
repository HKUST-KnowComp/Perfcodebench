#include "interface.h"

#include <cstdint>
#include <cstddef>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  
  const std::size_t probe_n = probe_ts.size();
  const std::size_t dim_n = dim_ts.size();
  
  if (probe_n == 0 || dim_n == 0) {
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
    std::size_t probe_idx = 0;
    
    // Skip probes before first dimension timestamp
    while (probe_idx < probe_n && p_ts[probe_idx] < d_ts[0]) {
      ++probe_idx;
    }
    
    // Main merge loop
    while (probe_idx < probe_n) {
      const uint32_t pts = p_ts[probe_idx];
      
      // Advance dim_idx to find largest dim_ts <= pts
      while (dim_idx + 1 < dim_n && d_ts[dim_idx + 1] <= pts) {
        ++dim_idx;
      }
      
      const uint32_t current_val = d_val[dim_idx];
      const uint32_t current_dim_ts_next = (dim_idx + 1 < dim_n) ? d_ts[dim_idx + 1] : UINT32_MAX;
      
      // Process all probes that map to current dim_idx
      uint64_t local_sum = 0;
      while (probe_idx < probe_n && p_ts[probe_idx] < current_dim_ts_next) {
        local_sum += static_cast<uint64_t>(p_wt[probe_idx]);
        ++probe_idx;
      }
      sum += local_sum * static_cast<uint64_t>(current_val);
    }
  }
  
  return sum;
}