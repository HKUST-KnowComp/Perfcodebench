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
    
    for (size_t i = 0; i < probe_size; ++i) {
      uint32_t p_ts = probe_ts[i];
      
      // Advance dim_idx to the last element <= p_ts
      // Since probe_ts is sorted, we don't need to reset dim_idx
      while (dim_idx + 1 < dim_size && dim_ts[dim_idx + 1] <= p_ts) {
        dim_idx++;
      }
      
      // As-of join: if p_ts < dim_ts[0], the baseline logic (it - begin - 1) 
      // would result in an underflow/out-of-bounds. 
      // However, the baseline uses std::upper_bound. If probe_ts[i] < dim_ts[0],
      // upper_bound returns dim_ts.begin(), and idx becomes (0 - 1) = size_t max.
      // To preserve the exact behavior of the baseline (which likely assumes 
      // probe_ts[i] >= dim_ts[0] or handles the wrap-around), we check the condition.
      if (p_ts >= dim_ts[0]) {
        current_sum += static_cast<uint64_t>(dim_values[dim_idx]) * probe_weights[i];
      } else {
        // This part mimics the baseline's behavior for probe_ts[i] < dim_ts[0]
        // where idx = static_cast<size_t>(-1).
        // In a real scenario, this would crash or access garbage. 
        // We use the same logic as the baseline provided.
        current_sum += static_cast<uint64_t>(dim_values[static_cast<size_t>(-1)]) * probe_weights[i];
      }
    }
    total_sum = current_sum;
  }
  return total_sum;
}