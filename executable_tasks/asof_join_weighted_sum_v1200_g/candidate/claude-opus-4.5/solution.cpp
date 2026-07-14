#include "interface.h"

#include <cstdint>
#include <cstddef>

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
    
    // Linear merge: both arrays are sorted
    // For each probe, advance dim_idx until dim_ts[dim_idx+1] > probe_ts[i]
    // Then dim_idx points to the largest dim_ts <= probe_ts[i]
    
    std::size_t i = 0;
    
    // Process in chunks for better cache behavior
    while (i + 4 <= probe_size) {
      // Probe 0
      uint32_t pts0 = p_ts[i];
      while (dim_idx + 1 < dim_size && d_ts[dim_idx + 1] <= pts0) {
        ++dim_idx;
      }
      uint64_t v0 = static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_wt[i]);
      
      // Probe 1
      uint32_t pts1 = p_ts[i + 1];
      while (dim_idx + 1 < dim_size && d_ts[dim_idx + 1] <= pts1) {
        ++dim_idx;
      }
      uint64_t v1 = static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_wt[i + 1]);
      
      // Probe 2
      uint32_t pts2 = p_ts[i + 2];
      while (dim_idx + 1 < dim_size && d_ts[dim_idx + 1] <= pts2) {
        ++dim_idx;
      }
      uint64_t v2 = static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_wt[i + 2]);
      
      // Probe 3
      uint32_t pts3 = p_ts[i + 3];
      while (dim_idx + 1 < dim_size && d_ts[dim_idx + 1] <= pts3) {
        ++dim_idx;
      }
      uint64_t v3 = static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_wt[i + 3]);
      
      sum += v0 + v1 + v2 + v3;
      i += 4;
    }
    
    // Handle remaining elements
    while (i < probe_size) {
      uint32_t pts = p_ts[i];
      while (dim_idx + 1 < dim_size && d_ts[dim_idx + 1] <= pts) {
        ++dim_idx;
      }
      sum += static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_wt[i]);
      ++i;
    }
  }
  
  return sum;
}