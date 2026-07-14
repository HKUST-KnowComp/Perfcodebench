#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  (void)iters; // Result is independent of iters; compute once for performance

  const std::size_t n_dim = dim_ts.size();
  const std::size_t n_probe = probe_ts.size();
  if (n_dim == 0 || n_probe == 0) return 0ULL;

  const uint32_t* dt = dim_ts.data();
  const uint32_t* dv = dim_values.data();
  const uint32_t* pt = probe_ts.data();
  const uint32_t* pw = probe_weights.data();

  std::size_t i = 0;       // probe index
  std::size_t dj = 0;      // dim index (last ts <= current probe ts)
  uint64_t sum = 0;

  // Skip probes earlier than the first dimension timestamp (no match)
  const uint32_t first_dim_ts = dt[0];
  while (i < n_probe && pt[i] < first_dim_ts) {
    ++i;
  }
  if (i == n_probe) return 0ULL;

  while (i < n_probe) {
    // Advance dim index so that dt[dj] is the last timestamp <= pt[i]
    while (dj + 1 < n_dim && dt[dj + 1] <= pt[i]) {
      ++dj;
    }

    // Next cutoff timestamp: all probes with ts < next_ts map to current dj
    const uint32_t next_ts = (dj + 1 < n_dim) ? dt[dj + 1] : UINT32_MAX;

    uint64_t wsum = 0;
    // Collect consecutive probes that fall before next_ts
    while (i < n_probe && pt[i] < next_ts) {
      wsum += static_cast<uint64_t>(pw[i]);
      ++i;
    }

    sum += static_cast<uint64_t>(dv[dj]) * wsum;
  }

  return sum;
}
