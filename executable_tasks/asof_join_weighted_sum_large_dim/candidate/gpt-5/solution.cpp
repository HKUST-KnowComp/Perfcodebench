#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int /*iters*/) {
  const std::size_t n_dim = dim_ts.size();
  const std::size_t n_probe = probe_ts.size();

  if (n_dim == 0 || n_probe == 0) {
    return 0ull;
  }

  // Check if probe_ts is nondecreasing to allow linear as-of merge.
  bool nondecreasing = true;
  for (std::size_t i = 1; i < n_probe; ++i) {
    if (probe_ts[i] < probe_ts[i - 1]) { nondecreasing = false; break; }
  }

  uint64_t sum = 0ull;

  const uint32_t* dt = dim_ts.data();
  const uint32_t* dv = dim_values.data();
  const uint32_t* pt = probe_ts.data();
  const uint32_t* pw = probe_weights.data();

  if (nondecreasing) {
    std::size_t j = 0;
    const std::size_t last = n_dim - 1;
    for (std::size_t i = 0; i < n_probe; ++i) {
      uint32_t t = pt[i];
      while (j < last && dt[j + 1] <= t) {
        ++j;
      }
      sum += static_cast<uint64_t>(dv[j]) * static_cast<uint64_t>(pw[i]);
    }
  } else {
    // Fallback: custom upper_bound for each probe timestamp.
    for (std::size_t i = 0; i < n_probe; ++i) {
      uint32_t t = pt[i];
      std::size_t lo = 0, hi = n_dim; // [lo, hi)
      while (lo < hi) {
        std::size_t mid = (lo + hi) >> 1;
        if (dt[mid] <= t) {
          lo = mid + 1;
        } else {
          hi = mid;
        }
      }
      std::size_t idx = lo - 1; // assumes there exists a match (dataset guarantees)
      sum += static_cast<uint64_t>(dv[idx]) * static_cast<uint64_t>(pw[i]);
    }
  }

  return sum;
}
