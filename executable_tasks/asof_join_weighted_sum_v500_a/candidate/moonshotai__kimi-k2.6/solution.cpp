#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

  if (iters <= 0) return 0;

  const std::size_t n_probe = probe_ts.size();
  const std::size_t n_dim = dim_ts.size();

  if (n_probe == 0 || n_dim == 0) return 0;

  const uint32_t* p_ts = probe_ts.data();
  const uint32_t* p_wt = probe_weights.data();
  const uint32_t* d_ts = dim_ts.data();
  const uint32_t* d_val = dim_values.data();

  const uint32_t* const d_end = d_ts + n_dim;
  const uint32_t* const p_end = p_ts + n_probe;

  uint64_t sum = 0;
  const uint32_t* dim_ptr = d_ts;
  const uint32_t* val_ptr = d_val;

  while (p_ts != p_end) {
    const uint32_t t = *p_ts;
    while ((dim_ptr + 1) != d_end && dim_ptr[1] <= t) {
      ++dim_ptr;
      ++val_ptr;
    }
    sum += static_cast<uint64_t>(*val_ptr) * static_cast<uint64_t>(*p_wt);
    ++p_ts;
    ++p_wt;
  }

  return sum * static_cast<uint64_t>(iters);
}
