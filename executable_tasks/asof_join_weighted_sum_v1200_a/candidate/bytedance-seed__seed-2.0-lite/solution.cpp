#include "interface.h"
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t dim_size = dim_ts.size();
  const std::size_t probe_size = probe_ts.size();
  
  // Precompute matching dimension indices once (linear scan of sorted timestamps)
  std::vector<std::size_t> probe_idx(probe_size);
  std::size_t j = 0;
  for (std::size_t i = 0; i < probe_size; ++i) {
    const uint32_t current_probe_ts = probe_ts[i];
    while (j + 1 < dim_size && dim_ts[j+1] <= current_probe_ts) {
      j++;
    }
    probe_idx[i] = j;
  }

  // Precompute weighted terms once to optimize inner loop
  std::vector<uint64_t> terms(probe_size);
  for (std::size_t i = 0; i < probe_size; ++i) {
    terms[i] = static_cast<uint64_t>(dim_values[probe_idx[i]]) * static_cast<uint64_t>(probe_weights[i]);
  }

  // Run benchmarked inner loop iters times
  uint64_t final_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (std::size_t i = 0; i < probe_size; ++i) {
      iter_sum += terms[i];
    }
    final_sum = iter_sum;
  }
  return final_sum;
}