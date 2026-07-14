#include "interface.h"

#include <cstddef>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  const size_t dim_size = dim_ts.size();
  const size_t probe_size = probe_ts.size();
  if (dim_size == 0 || probe_size == 0) return sum;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t dim_ptr = 0;
    for (size_t i = 0; i < probe_size; ++i) {
      const uint32_t current_probe_ts = probe_ts[i];
      while (dim_ptr + 1 < dim_size && dim_ts[dim_ptr + 1] <= current_probe_ts) {
        dim_ptr++;
      }
      sum += static_cast<uint64_t>(dim_values[dim_ptr]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}