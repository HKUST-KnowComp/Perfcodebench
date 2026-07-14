#include "interface.h"
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  const size_t nProbe = probe_ts.size();
  const size_t nDim   = dim_ts.size();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t dimIdx = 0;
    for (size_t probeIdx = 0; probeIdx < nProbe; ++probeIdx) {
      const uint32_t ts = probe_ts[probeIdx];
      while (dimIdx + 1 < nDim && dim_ts[dimIdx + 1] <= ts) {
        ++dimIdx;
      }
      sum += static_cast<uint64_t>(dim_values[dimIdx]) *
             static_cast<uint64_t>(probe_weights[probeIdx]);
    }
  }
  return sum;
}