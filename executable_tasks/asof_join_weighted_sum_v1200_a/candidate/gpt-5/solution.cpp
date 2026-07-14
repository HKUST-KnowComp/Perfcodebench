#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  // Preserve baseline behavior: if iters <= 0, return 0.
  if (iters <= 0) return 0;

  const std::size_t m = dim_ts.size();
  const std::size_t n = probe_ts.size();

  // Assume dim_ts and probe_ts are sorted ascending, per task statement.
  // Compute the sum for a single iteration using a linear sweep.
  const uint32_t* dts = dim_ts.data();
  const uint32_t* dval = dim_values.data();
  const uint32_t* pts = probe_ts.data();
  const uint32_t* pwt = probe_weights.data();

  std::size_t i = 0;  // probe index
  uint64_t sum = 0;

  if (m == 0 || n == 0) {
    return 0;
  }

  // Skip probes that occur before the first dimension timestamp (no match in as-of semantics).
  const uint32_t first_dim = dts[0];
  while (i < n && pts[i] < first_dim) {
    ++i;
  }

  // For each dim interval [dim_ts[j], dim_ts[j+1]) accumulate probe weights and multiply once.
  // Handle all intervals except the last one.
  for (std::size_t j = 0; j + 1 < m; ++j) {
    const uint32_t next_threshold = dts[j + 1];
    uint64_t wsum = 0;
    // Collect all probes whose timestamp is < next_threshold.
    while (i < n && pts[i] < next_threshold) {
      wsum += static_cast<uint64_t>(pwt[i]);
      ++i;
    }
    sum += static_cast<uint64_t>(dval[j]) * wsum;
  }

  // Handle the last interval [dim_ts[m-1], +inf): consume remaining probes.
  if (m > 0) {
    uint64_t wsum_last = 0;
    while (i < n) {
      wsum_last += static_cast<uint64_t>(pwt[i]);
      ++i;
    }
    sum += static_cast<uint64_t>(dval[m - 1]) * wsum_last;
  }

  // Baseline returns the sum of a single iteration; ignore remaining iterations.
  return sum;
}
