#include "interface.h"

#include <cstdint>
#include <vector>

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
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t dim_idx = 0;
    
    for (std::size_t i = 0; i < probe_size; ++i) {
      const uint32_t probe_t = probe_ts[i];
      
      // Advance dim_idx while next timestamp is still <= probe timestamp
      while (dim_idx + 1 < dim_size && dim_ts[dim_idx + 1] <= probe_t) {
        ++dim_idx;
      }
      
      // If first dim timestamp is greater than probe, skip (no valid match)
      if (dim_ts[dim_idx] > probe_t) {
        continue;
      }
      
      sum += static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  
  return sum;
}