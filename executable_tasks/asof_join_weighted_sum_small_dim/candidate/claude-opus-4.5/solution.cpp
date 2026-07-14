#include "interface.h"

#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

  const std::size_t probe_n = probe_ts.size();
  const std::size_t dim_n = dim_ts.size();

  if (probe_n == 0 || dim_n == 0) {
    return 0;
  }

  const uint32_t* __restrict__ p_ts = probe_ts.data();
  const uint32_t* __restrict__ p_wt = probe_weights.data();
  const uint32_t* __restrict__ d_ts = dim_ts.data();
  const uint32_t* __restrict__ d_val = dim_values.data();

  const uint32_t dim_last_ts = d_ts[dim_n - 1];

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t d = 0;
    std::size_t i = 0;

    // Process in blocks of 4 for better ILP
    const std::size_t probe_n4 = probe_n & ~std::size_t(3);

    while (i < probe_n4) {
      // Prefetch ahead
      __builtin_prefetch(p_ts + i + 32, 0, 0);
      __builtin_prefetch(p_wt + i + 32, 0, 0);

      uint32_t ts0 = p_ts[i];
      uint32_t ts1 = p_ts[i + 1];
      uint32_t ts2 = p_ts[i + 2];
      uint32_t ts3 = p_ts[i + 3];

      // Advance d for ts0
      if (ts0 <= dim_last_ts) {
        while (d + 1 < dim_n && d_ts[d + 1] <= ts0) ++d;
      } else {
        d = dim_n - 1;
      }
      uint32_t val0 = d_val[d];

      // Advance d for ts1
      if (ts1 <= dim_last_ts) {
        while (d + 1 < dim_n && d_ts[d + 1] <= ts1) ++d;
      } else {
        d = dim_n - 1;
      }
      uint32_t val1 = d_val[d];

      // Advance d for ts2
      if (ts2 <= dim_last_ts) {
        while (d + 1 < dim_n && d_ts[d + 1] <= ts2) ++d;
      } else {
        d = dim_n - 1;
      }
      uint32_t val2 = d_val[d];

      // Advance d for ts3
      if (ts3 <= dim_last_ts) {
        while (d + 1 < dim_n && d_ts[d + 1] <= ts3) ++d;
      } else {
        d = dim_n - 1;
      }
      uint32_t val3 = d_val[d];

      sum += static_cast<uint64_t>(val0) * static_cast<uint64_t>(p_wt[i]);
      sum += static_cast<uint64_t>(val1) * static_cast<uint64_t>(p_wt[i + 1]);
      sum += static_cast<uint64_t>(val2) * static_cast<uint64_t>(p_wt[i + 2]);
      sum += static_cast<uint64_t>(val3) * static_cast<uint64_t>(p_wt[i + 3]);

      i += 4;
    }

    // Scalar tail
    while (i < probe_n) {
      uint32_t ts = p_ts[i];
      if (ts <= dim_last_ts) {
        while (d + 1 < dim_n && d_ts[d + 1] <= ts) ++d;
      } else {
        d = dim_n - 1;
      }
      sum += static_cast<uint64_t>(d_val[d]) * static_cast<uint64_t>(p_wt[i]);
      ++i;
    }
  }

  return sum;
}
