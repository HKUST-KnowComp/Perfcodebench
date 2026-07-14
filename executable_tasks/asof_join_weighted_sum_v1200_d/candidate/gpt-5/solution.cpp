#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int /*iters*/) {
  const std::size_t n_dim = dim_ts.size();
  const std::size_t n_probe = probe_ts.size();
  if (n_dim == 0 || n_probe == 0) {
    return 0;
  }

  const uint32_t* dts = dim_ts.data();
  const uint32_t* dts_end = dts + n_dim;
  const uint32_t* dval = dim_values.data();
  const uint32_t* pts = probe_ts.data();
  const uint32_t* pwt = probe_weights.data();

  const uint32_t* dts_ptr = dts;
  const uint32_t* dval_ptr = dval;

  uint32_t last_value = 0;
  uint64_t sum = 0;

  for (std::size_t k = 0; k < n_probe; ++k) {
    const uint32_t t = pts[k];
    while (dts_ptr < dts_end && *dts_ptr <= t) {
      last_value = *dval_ptr;
      ++dts_ptr;
      ++dval_ptr;
    }
    sum += static_cast<uint64_t>(last_value) * static_cast<uint64_t>(pwt[k]);
  }

  return sum;
}
