#include "interface.h"

#include <cstddef>
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  // If no iterations requested or any input is empty, sum is zero.
  if (iters <= 0) return 0;
  const std::size_t nDim = dim_ts.size();
  const std::size_t nProbe = probe_ts.size();
  if (nDim == 0 || nProbe == 0) return 0;

  const uint32_t* dt = dim_ts.data();
  const uint32_t* dv = dim_values.data();
  const uint32_t* pt = probe_ts.data();
  const uint32_t* pw = probe_weights.data();

  // Two-pointer linear as-of join: O(nDim + nProbe)
  std::size_t j = 0; // index into dim arrays; invariant: dt[j] <= current probe ts when used
  uint32_t curVal = dv[0];

  uint64_t sum = 0;

  // Skip probes that are earlier than the first dimension timestamp (no match)
  std::size_t i = 0;
  const uint32_t firstDimTs = dt[0];
  while (i < nProbe && pt[i] < firstDimTs) {
    ++i;
  }

  for (; i < nProbe; ++i) {
    const uint32_t ts = pt[i];
    // Advance j to the last dim timestamp not greater than ts
    while ((j + 1) < nDim && dt[j + 1] <= ts) {
      ++j;
      curVal = dv[j];
    }
    sum += static_cast<uint64_t>(curVal) * static_cast<uint64_t>(pw[i]);
  }

  return sum;
}
