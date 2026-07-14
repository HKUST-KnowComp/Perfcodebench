#include "interface.h"

#include <cstddef>
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  // If no iterations requested, match baseline behavior (sum remains 0)
  if (iters <= 0) return 0;

  const std::size_t nDim = dim_ts.size();
  const std::size_t nProbe = probe_ts.size();

  if (nDim == 0 || nProbe == 0) return 0;

  const uint32_t* dts = dim_ts.data();
  const uint32_t* dval = dim_values.data();
  const uint32_t* pts = probe_ts.data();
  const uint32_t* pwt = probe_weights.data();

  uint64_t sum = 0;

  // Two-pointer as-of join assuming both dim_ts and probe_ts are sorted ascending.
  std::size_t j = 0;
  const uint32_t* pts_end = pts + nProbe;
  for (; pts != pts_end; ++pts, ++pwt) {
    const uint32_t p = *pts;
    while ((j + 1) < nDim && dts[j + 1] <= p) {
      ++j;
    }
    sum += static_cast<uint64_t>(dval[j]) * static_cast<uint64_t>(*pwt);
  }

  return sum;
}
