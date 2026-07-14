#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (dim_ts.empty() || probe_ts.empty()) {
    return 0;
  }

  uint64_t total_sum = 0;
  const size_t dim_size = dim_ts.size();
  const size_t probe_size = probe_ts.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    size_t dim_idx = 0;

    for (size_t i = 0; i < probe_size; ++i) {
      uint32_t p_ts = probe_ts[i];
      
      // Advance dim_idx to the largest index such that dim_ts[dim_idx] <= p_ts
      // Since probe_ts is sorted, dim_idx only moves forward.
      while (dim_idx + 1 < dim_size && dim_ts[dim_idx + 1] <= p_ts) {
        dim_idx++;
      }

      // Check if the current dim_idx is valid for the as-of join condition
      if (dim_ts[dim_idx] <= p_ts) {
        current_iter_sum += static_cast<uint64_t>(dim_values[dim_idx]) * probe_weights[i];
      }
    }
    total_sum += current_iter_sum;
  }

  return total_sum;
}