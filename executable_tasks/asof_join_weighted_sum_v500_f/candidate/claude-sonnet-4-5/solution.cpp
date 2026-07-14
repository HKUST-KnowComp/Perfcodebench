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
  
  if (probe_size == 0 || dim_size == 0) return 0;
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t dim_idx = 0;
    
    // Process in blocks of 4 for better ILP
    std::size_t i = 0;
    const std::size_t probe_size_aligned = (probe_size / 4) * 4;
    
    for (; i < probe_size_aligned; i += 4) {
      uint32_t pt0 = probe_ts[i];
      uint32_t pt1 = probe_ts[i + 1];
      uint32_t pt2 = probe_ts[i + 2];
      uint32_t pt3 = probe_ts[i + 3];
      
      // Advance dim_idx for first probe
      while (dim_idx + 1 < dim_size && dim_ts[dim_idx + 1] <= pt0) {
        ++dim_idx;
      }
      uint64_t val0 = static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[i]);
      
      // Advance for second probe
      while (dim_idx + 1 < dim_size && dim_ts[dim_idx + 1] <= pt1) {
        ++dim_idx;
      }
      uint64_t val1 = static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[i + 1]);
      
      // Advance for third probe
      while (dim_idx + 1 < dim_size && dim_ts[dim_idx + 1] <= pt2) {
        ++dim_idx;
      }
      uint64_t val2 = static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[i + 2]);
      
      // Advance for fourth probe
      while (dim_idx + 1 < dim_size && dim_ts[dim_idx + 1] <= pt3) {
        ++dim_idx;
      }
      uint64_t val3 = static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[i + 3]);
      
      sum += val0 + val1 + val2 + val3;
    }
    
    // Handle remaining elements
    for (; i < probe_size; ++i) {
      while (dim_idx + 1 < dim_size && dim_ts[dim_idx + 1] <= probe_ts[i]) {
        ++dim_idx;
      }
      sum += static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  
  return sum;
}