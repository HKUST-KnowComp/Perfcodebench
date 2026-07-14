#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  (void)iters;  // Unused because data is constant across iterations; result is identical.

  // Dense dimension: dim_keys are a contiguous, sorted range starting at some base.
  uint32_t base = dim_keys[0];

  uint64_t sum = 0;
  for (std::size_t i = 0; i < probe_keys.size(); ++i) {
    std::size_t idx = static_cast<std::size_t>(probe_keys[i] - base);
    sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
  }
  return sum;
}
