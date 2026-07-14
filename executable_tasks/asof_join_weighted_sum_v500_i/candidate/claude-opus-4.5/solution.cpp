#include "interface.h"

#include <cstddef>
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
    sum = 0;
    std::size_t d = 0;

    // Advance d so that dts[d] <= pts[0] and (d+1 >= dim_n or dts[d+1] > pts[0])
    // Since both are sorted, we do a linear merge.
    for (std::size_t i = 0; i < probe_n; ++i) {
      const uint32_t p = pts[i];

      // Prefetch ahead in probe arrays
      if ((i & 15) == 0 && i + 64 < probe_n) {
        __builtin_prefetch(&pts[i + 64], 0, 0);
        __builtin_prefetch(&pw[i + 64], 0, 0);
      }

      // Advance dim pointer while next dim timestamp is still <= probe timestamp
      while (d + 1 < dim_n && dts[d + 1] <= p) {
        ++d;
      }

      // dts[d] is the largest timestamp <= p (assuming dts[0] <= p always per problem semantics)
      sum += static_cast<uint64_t>(dval[d]) * static_cast<uint64_t>(pw[i]);
    }
  }

  return sum;
}
