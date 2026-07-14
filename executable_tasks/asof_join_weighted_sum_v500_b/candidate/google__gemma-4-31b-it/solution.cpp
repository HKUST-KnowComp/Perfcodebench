#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const size_t dim_size = dim_ts.size();
  const size_t probe_size = probe_ts.size();
  const uint32_t* d_ts = dim_ts.data();
  const uint32_t* d_val = dim_values.data();
  const uint32_t* p_ts = probe_ts.data();
  const uint32_t* p_w = probe_weights.data();

  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t dim_idx = 0;
    
    for (size_t i = 0; i < probe_size; ++i) {
      uint32_t target = p_ts[i];
      
      // Advance dim_idx to the last element <= target
      // Since probe_ts is sorted, we don't reset dim_idx
      while (dim_idx < dim_size && d_ts[dim_idx] <= target) {
        dim_idx++;
      }
      
      // The as-of join value is at dim_idx - 1
      // Note: The baseline assumes dim_ts[0] <= probe_ts[0] or handles underflow via size_t
      // based on the logic `it - dim_ts.begin() - 1`. 
      // If dim_idx is 0, it wraps to max size_t. We follow baseline behavior.
      current_sum += static_cast<uint64_t>(d_val[dim_idx - 1]) * static_cast<uint64_t>(p_w[i]);
    }
    total_sum = current_sum;
  }
  
  return total_sum;
}