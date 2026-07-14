#pragma once

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t total_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    std::size_t dim_idx = 0;
    for (std::size_t i = 0; i < probe_ts.size(); ++i) {
      while (dim_idx < dim_ts.size() && dim_ts[dim_idx] <= probe_ts[i]) {
        ++dim_idx;
      }
      const std::size_t idx = dim_idx - 1;
      sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
    total_sum += sum;
  }
  return total_sum;
}