#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const size_t dim_size = dim_ts.size();
  const size_t probe_size = probe_ts.size();
  
  if (dim_size == 0 || probe_size == 0) return 0;

  const uint32_t* d_ts = dim_ts.data();
  const uint32_t* d_val = dim_values.data();
  const uint32_t* p_ts = probe_ts.data();
  const uint32_t* p_w = probe_weights.data();

  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t d_idx = 0;
    
    for (size_t p_idx = 0; p_idx < probe_size; ++p_idx) {
      uint32_t target = p_ts[p_idx];
      
      // Advance d_idx to the last element <= target
      // Since probe_ts is sorted, we don't reset d_idx
      while (d_idx + 1 < dim_size && d_ts[d_idx + 1] <= target) {
        d_idx++;
      }
      
      current_sum += static_cast<uint64_t>(d_val[d_idx]) * static_cast<uint64_t>(p_w[p_idx]);
    }
    total_sum = current_sum;
  }

  return total_sum;
}