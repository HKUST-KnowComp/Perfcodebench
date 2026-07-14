#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t probe_n = probe_ts.size();
  const std::size_t dim_n = dim_ts.size();

  if (iters <= 0 || probe_n == 0) {
    return 0;
  }

  // Precompute the as-of match index for each probe timestamp once.
  // Inputs are sorted, so a single forward scan is faster than binary search
  // per probe and can be reused across iterations.
  std::vector<std::size_t> match_idx(probe_n);
  std::size_t j = 0;
  for (std::size_t i = 0; i < probe_n; ++i) {
    const uint32_t ts = probe_ts[i];
    while (j + 1 < dim_n && dim_ts[j + 1] <= ts) {
      ++j;
    }
    match_idx[i] = j;
  }

  const uint32_t* values = dim_values.data();
  const uint32_t* weights = probe_weights.data();
  const std::size_t* idx = match_idx.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    for (std::size_t i = 0; i < probe_n; ++i) {
      local_sum += static_cast<uint64_t>(values[idx[i]]) *
                   static_cast<uint64_t>(weights[i]);
    }
    sum = local_sum;
  }

  return sum;
}
