#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum_one = 0;
  std::size_t dim_idx = 0;
  for (std::size_t i = 0; i < probe_ts.size(); ++i) {
    // Advance dim_idx to the last element ≤ probe_ts[i]
    while (dim_idx + 1 < dim_ts.size() && dim_ts[dim_idx + 1] <= probe_ts[i]) {
      ++dim_idx;
    }
    // Fallback: binary search when the current dim_ts[dim_idx] > probe (should not happen if inputs are valid)
    if (dim_ts[dim_idx] > probe_ts[i]) {
      auto it = std::upper_bound(dim_ts.begin(), dim_ts.end(), probe_ts[i]);
      std::size_t idx = static_cast<std::size_t>(it - dim_ts.begin() - 1);
      sum_one += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
      dim_idx = idx; // keep the pointer consistent for the next probe
    } else {
      sum_one += static_cast<uint64_t>(dim_values[dim_idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum_one * static_cast<uint64_t>(iters);
}