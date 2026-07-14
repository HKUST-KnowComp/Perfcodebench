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
    
    // Merge-join: since both arrays are sorted, we can scan linearly
    for (std::size_t i = 0; i < probe_size; ++i) {
      const uint32_t pts = p_ts[i];
      
      // Advance dim_idx while next dim timestamp is still <= probe timestamp
      while (dim_idx + 1 < dim_size && d_ts[dim_idx + 1] <= pts) {
        ++dim_idx;
      }
      
      // dim_idx now points to the largest dim_ts <= probe_ts[i]
      sum += static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_wt[i]);
    }
  }
  
  return sum;
}