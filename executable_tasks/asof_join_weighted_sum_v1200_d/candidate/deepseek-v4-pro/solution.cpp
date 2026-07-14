#include "interface.h"

#include <cstddef>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t D = dim_ts.size();
  const std::size_t P = probe_ts.size();
  if (D == 0 || P == 0) return 0;

  // Pre‑compute the rightmost dimension index for each probe
  std::vector<std::size_t> idx_map(P);
  const uint32_t* d_ts = dim_ts.data();
  std::size_t j = 0;
  for (std::size_t i = 0; i < P; ++i) {
    const uint32_t p = probe_ts[i];
    while (j + 1 < D && d_ts[j + 1] <= p) {
      ++j;
    }
    idx_map[i] = j;
  }

  // Weighted sum over the required iterations
  uint64_t sum = 0;
  const uint32_t* d_val = dim_values.data();
  const uint32_t* p_w   = probe_weights.data();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < P; ++i) {
      sum += static_cast<uint64_t>(d_val[idx_map[i]]) * static_cast<uint64_t>(p_w[i]);
    }
  }
  return sum;
}