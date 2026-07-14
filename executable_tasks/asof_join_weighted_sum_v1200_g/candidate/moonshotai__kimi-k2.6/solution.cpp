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

  uint64_t sum = 0;
  std::size_t d = 0;
  const std::size_t dim_n = dim_ts.size();
  const std::size_t probe_n = probe_ts.size();

  for (std::size_t p = 0; p < probe_n; ++p) {
    const uint32_t t = pts[p];
    while (d + 1 < dim_n && dts[d + 1] <= t) {
      ++d;
    }
    sum += static_cast<uint64_t>(dval[d]) * static_cast<uint64_t>(pw[p]);
  }

  return sum;
}
