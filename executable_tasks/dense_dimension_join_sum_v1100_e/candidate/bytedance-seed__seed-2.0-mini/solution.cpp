#include "interface.h"

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t final_sum = 0;
  const size_t probe_count = probe_keys.size();

  // Early exit if there are no valid inputs
  if (probe_count == 0 || dim_keys.empty() || dim_values.size() != dim_keys.size()) {
    return 0;
  }

  const uint32_t dim_start = dim_keys[0];

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (size_t i = 0; i < probe_count; ++i) {
      const uint32_t key = probe_keys[i];
      const size_t idx = static_cast<size_t>(key - dim_start);
      current_sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
    final_sum = current_sum;
  }

  return final_sum;
}