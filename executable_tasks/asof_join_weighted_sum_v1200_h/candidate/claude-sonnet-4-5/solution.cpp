#include "interface.h"

#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  const std::size_t probe_size = probe_ts.size();
  const std::size_t dim_size = dim_ts.size();
  
  if (probe_size == 0 || dim_size == 0) return 0;
  
  const uint32_t* __restrict__ dim_ts_ptr = dim_ts.data();
  const uint32_t* __restrict__ dim_values_ptr = dim_values.data();
  const uint32_t* __restrict__ probe_ts_ptr = probe_ts.data();
  const uint32_t* __restrict__ probe_weights_ptr = probe_weights.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t dim_idx = 0;
    
    for (std::size_t i = 0; i < probe_size; ++i) {
      const uint32_t probe_t = probe_ts_ptr[i];
      
      // Advance dim_idx while we can still move forward and stay <= probe_t
      while (dim_idx + 1 < dim_size && dim_ts_ptr[dim_idx + 1] <= probe_t) {
        ++dim_idx;
      }
      
      sum += static_cast<uint64_t>(dim_values_ptr[dim_idx]) * static_cast<uint64_t>(probe_weights_ptr[i]);
    }
  }
  return sum;
}