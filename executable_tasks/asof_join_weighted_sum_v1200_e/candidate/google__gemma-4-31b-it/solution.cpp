#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t total_sum = 0;
  const size_t dim_size = dim_ts.size();
  const size_t probe_size = probe_ts.size();

  if (dim_size == 0 || probe_size == 0) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t dim_idx = 0;
    
    for (size_t i = 0; i < probe_size; ++i) {
      uint32_t p_ts = probe_ts[i];
      
      // Advance dim_idx to the last element <= p_ts
      // Since probe_ts is sorted, we don't need to reset dim_idx
      while (dim_idx + 1 < dim_size && dim_ts[dim_idx + 1] <= p_ts) {
        dim_idx++;
      }
      
      // The as-of join requires the latest timestamp not greater than probe_ts.
      // If dim_ts[0] > probe_ts, the baseline logic (it - begin - 1) would result 
      // in an underflow/out-of-bounds. However, based on the baseline's 
      // behavior with std::upper_bound, we assume dim_ts[0] <= probe_ts[0].
      current_sum += static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
    total_sum = current_sum;
  }
  return total_sum;
}