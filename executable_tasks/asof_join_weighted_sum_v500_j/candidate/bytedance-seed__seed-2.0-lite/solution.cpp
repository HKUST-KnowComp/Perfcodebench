#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t pre_sum = 0;
  if (!probe_ts.empty() && !dim_ts.empty()) {
    const size_t dim_size = dim_ts.size();
    size_t dim_ptr = 0;
    for (size_t i = 0; i < probe_ts.size(); ++i) {
      const uint32_t current_probe = probe_ts[i];
      while (dim_ptr + 1 < dim_size && dim_ts[dim_ptr + 1] <= current_probe) {
        dim_ptr++;
      }
      pre_sum += static_cast<uint64_t>(dim_values[dim_ptr]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = pre_sum;
  }
  return sum;
}