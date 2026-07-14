#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (iters <= 0) return 0;

  const std::size_t n_dim = dim_ts.size();
  const std::size_t n_probe = probe_ts.size();

  const uint32_t* dts = dim_ts.data();
  const uint32_t* dvs = dim_values.data();
  const uint32_t* pts = probe_ts.data();
  const uint32_t* pws = probe_weights.data();

  uint64_t per_iter_sum = 0;

  std::size_t di = 0;          // index into dim_ts/dim_values
  uint32_t current_val = 0;    // current as-of value (0 if none <= current probe)

  for (std::size_t pi = 0; pi < n_probe; ++pi) {
    const uint32_t t = pts[pi];
    while (di < n_dim && dts[di] <= t) {
      current_val = dvs[di];
      ++di;
    }
    per_iter_sum += static_cast<uint64_t>(current_val) * static_cast<uint64_t>(pws[pi]);
  }

  return per_iter_sum * static_cast<uint64_t>(iters);
}
