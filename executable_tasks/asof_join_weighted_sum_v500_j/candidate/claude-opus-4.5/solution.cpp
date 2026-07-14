#include "interface.h"

#include <cstdint>
#include <cstddef>

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

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t dim_idx = 0;

    // Advance dim_idx so that dim_ts[dim_idx] <= probe_ts[0] and dim_ts[dim_idx+1] > probe_ts[0]
    // i.e., find the largest dim_idx where dim_ts[dim_idx] <= probe_ts[0]
    // Since probe_ts is sorted, we can do a merge-style scan.

    std::size_t i = 0;

    // Process in chunks for better ILP
    constexpr std::size_t UNROLL = 4;
    const std::size_t probe_end4 = (probe_n / UNROLL) * UNROLL;

    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;

    while (i < probe_end4) {
      // Prefetch ahead
      __builtin_prefetch(p_ts + i + 64, 0, 0);
      __builtin_prefetch(p_wt + i + 64, 0, 0);

      // Process 4 probes
      for (std::size_t k = 0; k < UNROLL; ++k) {
        const uint32_t pts = p_ts[i + k];
        // Advance dim_idx while next dim timestamp is still <= probe timestamp
        while (dim_idx + 1 < dim_n && d_ts[dim_idx + 1] <= pts) {
          ++dim_idx;
        }
        const uint64_t val = d_val[dim_idx];
        const uint64_t wt = p_wt[i + k];
        switch (k) {
          case 0: s0 += val * wt; break;
          case 1: s1 += val * wt; break;
          case 2: s2 += val * wt; break;
          case 3: s3 += val * wt; break;
        }
      }
      i += UNROLL;
    }

    sum = s0 + s1 + s2 + s3;

    // Handle remaining probes
    while (i < probe_n) {
      const uint32_t pts = p_ts[i];
      while (dim_idx + 1 < dim_n && d_ts[dim_idx + 1] <= pts) {
        ++dim_idx;
      }
      sum += static_cast<uint64_t>(d_val[dim_idx]) * static_cast<uint64_t>(p_wt[i]);
      ++i;
    }
  }

  return sum;
}
