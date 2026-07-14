#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t total_sum = 0;
  const std::size_t n_probe = probe_ts.size();
  const std::size_t n_dim = dim_ts.size();

  if (n_dim == 0 || n_probe == 0) {
    return 0;
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    std::size_t dim_idx = 0;

    for (std::size_t i = 0; i < n_probe; ++i) {
      const uint32_t p_ts = probe_ts[i];
      
      // Advance dim_idx to the largest index such that dim_ts[dim_idx] <= p_ts
      // Since probe_ts is sorted, dim_idx only moves forward.
      while (dim_idx + 1 < n_dim && dim_ts[dim_idx + 1] <= p_ts) {
        dim_idx++;
      }

      // Check if the current dim_ts is valid for the probe_ts (as-of join condition)
      if (dim_ts[dim_idx] <= p_ts) {
        current_iter_sum += static_cast<uint64_t>(dim_values[dim_idx]) * probe_weights[i];
      }
    }
    total_sum += current_iter_sum;
  }

  // The task asks for the sum. Based on the baseline, it returns the sum of the last iteration.
  // However, the baseline code actually resets sum = 0 inside the loop and returns the final sum.
  // To match the baseline's behavior exactly (returning the sum of the LAST iteration):
  // We should return current_iter_sum from the last loop. 
  // But the baseline code 'sum = 0' inside the loop means it returns the sum of the final iteration.
  
  // Re-evaluating baseline: 
  // sum = 0; (inside loop)
  // sum += ... (inside loop)
  // return sum; (outside loop)
  // This means it returns the sum of the LAST iteration only.
  
  // Let's correct the logic to match the baseline's exact return value.
  // The baseline returns the sum of the last iteration.
  
  // Re-implementing to match baseline behavior exactly:
  uint64_t final_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    final_sum = 0;
    std::size_t d_idx = 0;
    for (std::size_t i = 0; i < n_probe; ++i) {
      const uint32_t p_ts = probe_ts[i];
      while (d_idx + 1 < n_dim && dim_ts[d_idx + 1] <= p_ts) {
        d_idx++;
      }
      if (dim_ts[d_idx] <= p_ts) {
        final_sum += static_cast<uint64_t>(dim_values[d_idx]) * probe_weights[i];
      }
    }
  }
  return final_sum;
}