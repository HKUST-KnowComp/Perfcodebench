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

  if (dim_size == 0) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t dim_idx = 0;
    
    const uint32_t* d_ts_ptr = dim_ts.data();
    const uint32_t* d_val_ptr = dim_values.data();
    const uint32_t* p_ts_ptr = probe_ts.data();
    const uint32_t* p_w_ptr = probe_weights.data();

    for (size_t i = 0; i < probe_size; ++i) {
      uint32_t p_ts = p_ts_ptr[i];
      
      // Advance dim_idx to the last element <= p_ts
      // Since probe_ts is sorted, we only move forward
      while (dim_idx + 1 < dim_size && d_ts_ptr[dim_idx + 1] <= p_ts) {
        dim_idx++;
      }
      
      // The as-of join requires the latest timestamp not greater than probe_ts.
      // If the first dim_ts is already > p_ts, the baseline logic (it - begin - 1) 
      // would result in an underflow/out-of-bounds. However, the baseline 
      // logic implies dim_ts[0] <= probe_ts[0] is expected or handled by the 
      // specific dataset. To match baseline behavior exactly:
      if (d_ts_ptr[dim_idx] <= p_ts) {
        current_sum += static_cast<uint64_t>(d_val_ptr[dim_idx]) * static_cast<uint64_t>(p_w_ptr[i]);
      } else {
        // This case handles when probe_ts[i] < dim_ts[0].
        // In the baseline: upper_bound returns begin(), idx = 0 - 1 = size_t max.
        // This would typically crash. Assuming valid input where probe_ts[i] >= dim_ts[0].
      }
    }
    total_sum = current_sum;
  }
  return total_sum;
}