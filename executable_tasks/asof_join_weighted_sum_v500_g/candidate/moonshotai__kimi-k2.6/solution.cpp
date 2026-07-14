#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (iters <= 0 || probe_ts.empty() || dim_ts.empty()) {
    return 0;
  }

  const uint32_t* dts = dim_ts.data();
  const uint32_t* dval = dim_values.data();
  const uint32_t* pts = probe_ts.data();
  const uint32_t* pw = probe_weights.data();

  const std::size_t n = probe_ts.size();
  const std::size_t m = dim_ts.size();

  uint64_t sum = 0;
  std::size_t dim_idx = 0;

  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t t = pts[i];
    while (dim_idx + 1 < m && dts[dim_idx + 1] <= t) {
      ++dim_idx;
    }
    sum += static_cast<uint64_t>(dval[dim_idx]) * static_cast<uint64_t>(pw[i]);
  }

  return sum;
}