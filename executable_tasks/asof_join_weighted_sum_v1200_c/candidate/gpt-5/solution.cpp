#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int /*iters*/) {
  const std::size_t nDim = dim_ts.size();
  const std::size_t nProbe = probe_ts.size();

  if (nDim == 0 || nProbe == 0) {
    return 0ULL;
  }

  const uint32_t* dimTs = dim_ts.data();
  const uint32_t* dimVals = dim_values.data();
  const uint32_t* probeTs = probe_ts.data();
  const uint32_t* probeW = probe_weights.data();

  uint64_t sum = 0ULL;
  std::size_t j = 0;
  uint32_t current_val = 0;

  for (std::size_t i = 0; i < nProbe; ++i) {
    const uint32_t p = probeTs[i];
    // Advance through dim_ts while dim_ts[j] <= p, tracking the latest value
    while (j < nDim && dimTs[j] <= p) {
      current_val = dimVals[j];
      ++j;
    }
    sum += static_cast<uint64_t>(current_val) * static_cast<uint64_t>(probeW[i]);
  }

  return sum;
}
