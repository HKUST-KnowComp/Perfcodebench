#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t n_probe = probe_ts.size();
  if (n_probe == 0) return 0;
  const std::size_t n_dim = dim_ts.size();
  
  std::size_t dim_ptr = 0;
  std::vector<uint64_t> products(n_probe);
  for (std::size_t i = 0; i < n_probe; ++i) {
    const uint32_t current_probe = probe_ts[i];
    while (dim_ptr + 1 < n_dim && dim_ts[dim_ptr + 1] <= current_probe) {
      ++dim_ptr;
    }
    products[i] = static_cast<uint64_t>(dim_values[dim_ptr]) * static_cast<uint64_t>(probe_weights[i]);
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (const uint64_t p : products) {
      sum += p;
    }
  }
  return sum;
}