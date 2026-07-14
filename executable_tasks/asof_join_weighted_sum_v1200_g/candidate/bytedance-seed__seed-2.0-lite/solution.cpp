#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t final_sum = 0;
  const size_t dim_size = dim_ts.size();
  const size_t probe_count = probe_ts.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    size_t ptr = 0;
    for (size_t i = 0; i < probe_count; ++i) {
      const uint32_t current_probe = probe_ts[i];
      while (ptr + 1 < dim_size && dim_ts[ptr + 1] <= current_probe) {
        ptr++;
      }
      sum += static_cast<uint64_t>(dim_values[ptr]) * static_cast<uint64_t>(probe_weights[i]);
    }
    final_sum = sum;
  }
  return final_sum;
}