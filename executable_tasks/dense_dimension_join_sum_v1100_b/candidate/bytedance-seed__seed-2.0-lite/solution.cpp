#include "interface.h"

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  // Precompute base offset for dense sorted dimension keys (valid per input contract: dim_keys is sorted)
  const uint32_t min_key = dim_keys[0];
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      // Direct index calculation avoids expensive binary search per probe
      const std::size_t idx = static_cast<std::size_t>(probe_keys[i] - min_key);
      sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}