#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  
  const size_t dim_size = dim_ts.size();
  const size_t probe_size = probe_ts.size();

  if (dim_size == 0 || probe_size == 0) {
    return 0;
  }

  uint64_t total_sum = 0;
  const uint32_t* d_ts = dim_ts.data();
  const uint32_t* d_val = dim_values.data();
  const uint32_t* p_ts = probe_ts.data();
  const uint32_t* p_w = probe_weights.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t dim_idx = 0;

    // Two-pointer approach: O(P + D)
    // Since probe_ts is sorted, we only ever move dim_idx forward.
    for (size_t i = 0; i < probe_size; ++i) {
      uint32_t pts = p_ts[i];
      
      // Advance dim_idx to the largest index such that dim_ts[dim_idx] <= probe_ts[i]
      // We check the next element to see if it still satisfies the condition.
      while (dim_idx + 1 < dim_size && d_ts[dim_idx + 1] <= pts) {
        dim_idx++;
      }
      
      current_sum += static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_w[i]);
    }
    total_sum = current_sum;
  }

  return total_sum;
}