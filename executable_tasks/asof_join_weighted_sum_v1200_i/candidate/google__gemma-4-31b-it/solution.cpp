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
      
      // The as-of join requires the latest timestamp not greater than probe_ts[i].
      // If dim_ts[0] > probe_ts[0], the baseline logic (it - begin - 1) would result 
      // in an underflow/out-of-bounds. However, based on the baseline's behavior 
      // and typical as-of join definitions, we assume dim_ts[0] <= probe_ts[0] 
      // or handle the index carefully. 
      // The baseline: std::upper_bound returns begin() if all elements > target, 
      // then idx = 0 - 1 = size_t max. 
      // To preserve exact baseline behavior for edge cases:
      if (d_ts[0] > target) {
        // This mimics the baseline's behavior of accessing dim_values[static_cast<size_t>(-1)]
        // which is technically UB but we follow the logic if it's expected.
        // In a real scenario, we'd check if d_idx is valid.
        current_sum += static_cast<uint64_t>(d_val[static_cast<size_t>(-1)]) * p_w[p_idx];
      } else {
        current_sum += static_cast<uint64_t>(d_val[d_idx]) * p_w[p_idx];
      }
    }
    total_sum = current_sum;
  }
  
  return total_sum;
}