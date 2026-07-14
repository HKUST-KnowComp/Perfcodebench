#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  const std::size_t n_dim = dim_ts.size();
  const std::size_t n_probe = probe_ts.size();
  const uint32_t* dim_ts_ptr = dim_ts.data();
  const uint32_t* dim_values_ptr = dim_values.data();
  const uint32_t* probe_ts_ptr = probe_ts.data();
  const uint32_t* probe_weights_ptr = probe_weights.data();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t j = 0;
    for (std::size_t i = 0; i < n_probe; ++i) {
      // Advance j while next dim timestamp is not greater than current probe
      while (j + 1 < n_dim && dim_ts_ptr[j + 1] <= probe_ts_ptr[i]) {
        ++j;
      }
      sum += static_cast<uint64_t>(dim_values_ptr[j]) * static_cast<uint64_t>(probe_weights_ptr[i]);
    }
  }
  return sum;
}
