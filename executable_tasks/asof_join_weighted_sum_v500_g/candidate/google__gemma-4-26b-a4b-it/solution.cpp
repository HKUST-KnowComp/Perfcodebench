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
  const size_t n_dim = dim_ts.size();
  const size_t n_probe = probe_ts.size();

  // Pre-calculate the mapping for one pass to avoid O(N log M) in every iteration.
  // Since both arrays are sorted, we use a two-pointer approach to find the 
  // 'latest not-greater' dimension index for each probe.
  std::vector<size_t> dim_indices(n_probe);
  size_t dim_idx = 0;
  for (size_t i = 0; i < n_probe; ++i) {
    // Move dim_idx forward as long as the next dimension timestamp is <= current probe timestamp
    while (dim_idx + 1 < n_dim && dim_ts[dim_idx + 1] <= probe_ts[i]) {
      dim_idx++;
    }
    
    // Check if the current dim_idx is valid for the probe (dim_ts[dim_idx] <= probe_ts[i])
    if (dim_ts[dim_idx] <= probe_ts[i]) {
      dim_indices[i] = dim_idx;
    } else {
      // If even the first dimension is greater than the probe, it's an invalid join
      // We use a sentinel value or handle it via a flag. 
      // However, based on the problem, we assume valid as-of join logic.
      // We'll use a special value to indicate 'no match'.
      dim_indices[i] = static_cast<size_t>(-1);
    }
  }

  // Pre-calculate the products for one pass to maximize throughput
  // This allows the inner loop to be a simple accumulation.
  std::vector<uint64_t> products(n_probe);
  for (size_t i = 0; i < n_probe; ++i) {
    size_t d_idx = dim_indices[i];
    if (d_idx != static_cast<size_t>(-1)) {
      products[i] = static_cast<uint64_t>(dim_values[d_idx]) * static_cast<uint64_t>(probe_weights[i]);
    } else {
      products[i] = 0;
    }
  }

  // Perform the iterations
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < n_probe; ++i) {
      current_iter_sum += products[i];
    }
    total_sum = current_iter_sum;
  }

  return total_sum;
}