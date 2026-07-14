#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  const size_t m = dim_ts.size();
  const size_t n = probe_ts.size();
  const uint32_t* d_ts = dim_ts.data();
  const uint32_t* d_val = dim_values.data();
  const uint32_t* p_ts = probe_ts.data();
  const uint32_t* p_w = probe_weights.data();
  const uint32_t* d_ts_end = d_ts + m;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const uint32_t* cur_d_ts = d_ts;
    const uint32_t* cur_d_val = d_val;
    for (size_t i = 0; i < n; ++i) {
      uint32_t pt = p_ts[i];
      while (cur_d_ts + 1 < d_ts_end && *(cur_d_ts + 1) <= pt) {
        ++cur_d_ts;
        ++cur_d_val;
      }
      sum += static_cast<uint64_t>(*cur_d_val) * p_w[i];
    }
  }
  return sum;
}