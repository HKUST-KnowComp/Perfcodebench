#include "interface.h"

#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t dim_size = dim_ts.size();
  const std::size_t probe_size = probe_ts.size();
  
  if (probe_size == 0 || dim_size == 0) {
    return 0;
  }
  
  const uint32_t* __restrict__ dim_ts_ptr = dim_ts.data();
  const uint32_t* __restrict__ dim_val_ptr = dim_values.data();
  const uint32_t* __restrict__ probe_ts_ptr = probe_ts.data();
  const uint32_t* __restrict__ probe_wt_ptr = probe_weights.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t dim_idx = 0;
    
    // Find initial position: largest dim_ts[dim_idx] <= probe_ts[0]
    // We need dim_ts[dim_idx] <= probe_ts[i] and dim_ts[dim_idx+1] > probe_ts[i]
    // Start by finding position for first probe
    uint32_t pt = probe_ts_ptr[0];
    
    // Advance dim_idx while next element is still <= pt
    while (dim_idx + 1 < dim_size && dim_ts_ptr[dim_idx + 1] <= pt) {
      ++dim_idx;
    }
    
    uint32_t current_val = dim_val_ptr[dim_idx];
    sum += static_cast<uint64_t>(current_val) * static_cast<uint64_t>(probe_wt_ptr[0]);
    
    for (std::size_t i = 1; i < probe_size; ++i) {
      pt = probe_ts_ptr[i];
      
      // Since probe_ts is sorted, we only need to advance dim_idx forward
      while (dim_idx + 1 < dim_size && dim_ts_ptr[dim_idx + 1] <= pt) {
        ++dim_idx;
        current_val = dim_val_ptr[dim_idx];
      }
      
      sum += static_cast<uint64_t>(current_val) * static_cast<uint64_t>(probe_wt_ptr[i]);
    }
  }
  
  return sum;
}