#include "interface.h"

#include <cstddef>
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t dim_n = dim_ts.size();
  const std::size_t probe_n = probe_ts.size();
  
  if (probe_n == 0 || dim_n == 0) {
    return 0;
  }

  const uint32_t* __restrict dim_ts_ptr = dim_ts.data();
  const uint32_t* __restrict dim_val_ptr = dim_values.data();
  const uint32_t* __restrict probe_ts_ptr = probe_ts.data();
  const uint32_t* __restrict probe_wt_ptr = probe_weights.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t dim_idx = 0;
    
    // Linear merge join: both arrays are sorted
    // For each probe, advance dim_idx until dim_ts[dim_idx+1] > probe_ts[i]
    // Then dim_idx is the largest index where dim_ts[dim_idx] <= probe_ts[i]
    
    std::size_t i = 0;
    const std::size_t dim_last = dim_n - 1;
    
    // Process probes in chunks for better ILP
    for (; i < probe_n; ++i) {
      const uint32_t pts = probe_ts_ptr[i];
      
      // Advance dim_idx to find largest dim_ts <= pts
      while (dim_idx < dim_last && dim_ts_ptr[dim_idx + 1] <= pts) {
        ++dim_idx;
      }
      
      sum += static_cast<uint64_t>(dim_val_ptr[dim_idx]) * 
             static_cast<uint64_t>(probe_wt_ptr[i]);
    }
  }

  return sum;
}