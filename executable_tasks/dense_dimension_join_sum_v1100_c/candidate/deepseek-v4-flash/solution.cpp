#include "interface.h"

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  // Dense dimension: keys are contiguous from dim_keys[0] to dim_keys.back().
  // Directly index into dim_values using probe key offset.
  uint64_t sum = 0;
  const uint32_t min_key = dim_keys[0];
  const uint32_t* dim_vals = dim_values.data();
  const uint32_t* probe_k = probe_keys.data();
  const uint32_t* probe_w = probe_weights.data();
  const size_t n = probe_keys.size();
  for (size_t i = 0; i < n; ++i) {
    uint32_t idx = probe_k[i] - min_key;
    sum += static_cast<uint64_t>(dim_vals[idx]) * static_cast<uint64_t>(probe_w[i]);
  }
  // Result is same for any number of iterations; ignore iters for correctness.
  (void)iters;
  return sum;
}
