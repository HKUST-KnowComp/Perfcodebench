#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

  const size_t probeN = probe_ts.size();
  const size_t dimN   = dim_ts.size();

  // fast path: if either side is empty, the weighted sum is zero
  if (probeN == 0 || dimN == 0) {
    return 0;
  }

  const uint32_t * const __restrict dts  = dim_ts.data();
  const uint32_t * const __restrict dval = dim_values.data();
  const uint32_t * const __restrict pts  = probe_ts.data();
  const uint32_t * const __restrict pw   = probe_weights.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;

    // process 4 probes at once in a manually unrolled loop
    size_t i = 0;
    for ( ; i + 3 < probeN; i += 4) {
      uint32_t t0 = pts[i];
      uint32_t t1 = pts[i+1];
      uint32_t t2 = pts[i+2];
      uint32_t t3 = pts[i+3];

      // find latest dim_ts <= tx
      size_t idx0 = (std::upper_bound(dts, dts + dimN, t0) - dts) - 1;
      size_t idx1 = (std::upper_bound(dts, dts + dimN, t1) - dts) - 1;
      size_t idx2 = (std::upper_bound(dts, dts + dimN, t2) - dts) - 1;
      size_t idx3 = (std::upper_bound(dts, dts + dimN, t3) - dts) - 1;

      uint64_t v0 = dval[idx0];
      uint64_t v1 = dval[idx1];
      uint64_t v2 = dval[idx2];
      uint64_t v3 = dval[idx3];
      uint64_t w0 = pw[i];
      uint64_t w1 = pw[i+1];
      uint64_t w2 = pw[i+2];
      uint64_t w3 = pw[i+3];

      sum += v0 * w0 + v1 * w1;
      sum += v2 * w2 + v3 * w3;
    }

    // mop up residuals < 4
    for ( ; i < probeN; ++i) {
      size_t idx = (std::upper_bound(dts, dts + dimN, pts[i]) - dts) - 1;
      sum += static_cast<uint64_t>(dval[idx]) * static_cast<uint64_t>(pw[i]);
    }
  }

  return sum;
}