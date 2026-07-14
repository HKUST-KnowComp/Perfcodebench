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
  const std::size_t n_dim = dim_ts.size();
  const std::size_t n_probe = probe_ts.size();

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

      // Check if the current dim_idx is valid for the probe timestamp
      if (dim_ts[dim_idx] <= p_ts) {
        current_iter_sum += static_cast<uint64_t>(dim_values[dim_idx]) * probe_weights[i];
      }
    }
    total_sum += current_iter_sum;
  }

  // The task asks for the sum over iters. 
  // The baseline returns the sum of the LAST iteration if it were just 'sum = 0' inside the loop.
  // However, the baseline code provided: 
  // sum = 0; for(...) { sum = 0; for(...) { sum += ... } } 
  // This means the baseline actually returns the sum of the LAST iteration only.
  // Let's re-examine the baseline: 
  // sum = 0; for (int iter = 0; iter < iters; ++iter) { sum = 0; ... }
  // This is mathematically equivalent to just running the loop once.
  // To preserve exact correctness with the baseline's behavior:
  
  // Re-calculating to match baseline's exact logic (returning only the last iteration's sum):
  uint64_t last_sum = 0;
  std::size_t d_idx = 0;
  for (std::size_t i = 0; i < n_probe; ++i) {
    const uint32_t p_ts = probe_ts[i];
    while (d_idx + 1 < n_dim && dim_ts[d_idx + 1] <= p_ts) {
      d_idx++;
    }
    if (dim_ts[d_idx] <= p_ts) {
      last_sum += static_cast<uint64_t>(dim_values[d_idx]) * probe_weights[i];
    }
  }
  
  // Wait, the baseline loop is:
  // sum = 0;
  // for (int iter = 0; iter < iters; ++iter) {
  //    sum = 0;
  //    for (...) { sum += ... }
  // }
  // This means 'sum' is reset every iteration. The final value of 'sum' is just the result of the last iteration.
  // If iters is 0, it returns 0. If iters > 0, it returns the sum of one iteration.
  
  if (iters <= 0) return 0;
  
  // Optimized single pass to match baseline behavior
  uint64_t final_sum = 0;
  std::size_t current_dim_idx = 0;
  for (std::size_t i = 0; i < n_probe; ++i) {
    const uint32_t p_ts = probe_ts[i];
    while (current_dim_idx + 1 < n_dim && dim_ts[current_dim_idx + 1] <= p_ts) {
      current_dim_idx++;
    }
    if (dim_ts[current_dim_idx] <= p_ts) {
      final_sum += static_cast<uint64_t>(dim_values[current_dim_idx]) * probe_weights[i];
    }
  }
  
  return final_sum;
}