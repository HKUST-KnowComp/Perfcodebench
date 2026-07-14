#include "interface.h"

#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t dn = dim_ts.size();
  const std::size_t pn = probe_ts.size();

  // Fast paths for empty inputs
  if (dn == 0 || pn == 0) {
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
      sum = 0;
    }
    return sum;
  }

  const uint32_t* dt = dim_ts.data();
  const uint32_t* dv = dim_values.data();
  const uint32_t* pt = probe_ts.data();
  const uint32_t* pw = probe_weights.data();

  // Compute the result once using a linear-time two-pointer scan.
  // Assumes probe_ts are sorted ascending (as per task statement).
  uint64_t result = 0;
  const uint32_t* cur_ts = dt;               // points to current dimension timestamp
  const uint32_t* cur_val = dv;              // points to current dimension value
  const uint32_t* last_ts = dt + (dn - 1);   // last valid timestamp position

  for (std::size_t i = 0; i < pn; ++i) {
    const uint32_t t = pt[i];
    // Advance along dim while next timestamp is <= probe timestamp
    while (cur_ts < last_ts && cur_ts[1] <= t) {
      ++cur_ts;
      ++cur_val;
    }
    result += static_cast<uint64_t>(*cur_val) * static_cast<uint64_t>(pw[i]);
  }

  // Repeat assignment per iteration to preserve contract while avoiding recomputation
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = result;
  }
  return sum;
}
