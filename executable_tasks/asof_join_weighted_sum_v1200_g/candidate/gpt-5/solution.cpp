#include "interface.h"

#include <cstddef>
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  // Preserve baseline behavior: if no iterations requested, return 0.
  if (iters <= 0) return 0;

  const std::size_t dim_n = dim_ts.size();
  const std::size_t probe_n = probe_ts.size();

  // If either dimension or probes is empty, nothing contributes to the sum.
  if (dim_n == 0 || probe_n == 0) return 0;

  const uint32_t* dt = dim_ts.data();
  const uint32_t* dv = dim_values.data();
  const uint32_t* pt = probe_ts.data();
  const uint32_t* pw = probe_weights.data();

  // Two-pointer scan: assumes probe_ts and dim_ts are sorted in non-decreasing order.
  std::size_t j = 0;
  uint64_t sum = 0;

  for (std::size_t i = 0; i < probe_n; ++i) {
    const uint32_t t = pt[i];
    // Advance j to the last dim_ts <= current probe timestamp.
    while ((j + 1) < dim_n && dt[j + 1] <= t) {
      ++j;
    }
    sum += static_cast<uint64_t>(dv[j]) * static_cast<uint64_t>(pw[i]);
  }

  // Baseline recomputed the same value each iter but returned the last one.
  // Compute once and return.
  return sum;
}
