#include "interface.h"

#include <algorithm>
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

  const std::size_t dim_n = dim_ts.size();
  const std::size_t probe_n = probe_ts.size();

  if (dim_n == 0 || probe_n == 0) {
    return 0;
  }

  const uint32_t* __restrict__ dts = dim_ts.data();
  const uint32_t* __restrict__ dval = dim_values.data();
  const uint32_t* __restrict__ pts = probe_ts.data();
  const uint32_t* __restrict__ pw = probe_weights.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t d = 0;

    for (std::size_t p = 0; p < probe_n; ++p) {
      const uint32_t t = pts[p];

      // Advance d so that dts[d] <= t and (d+1 >= dim_n or dts[d+1] > t)
      while (d + 1 < dim_n && dts[d + 1] <= t) {
        ++d;
      }

      // Prefetch next dim_values to hide latency
      if (d + 8 < dim_n) {
        __builtin_prefetch(&dval[d + 8], 0, 0);
      }

      local_sum += static_cast<uint64_t>(dval[d]) * static_cast<uint64_t>(pw[p]);
    }

    sum = local_sum;
  }

  return sum;
}
