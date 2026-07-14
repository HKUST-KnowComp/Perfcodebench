#include "interface.h"

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  (void)dim_keys;
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      sum += static_cast<uint64_t>(dim_values[static_cast<std::size_t>(probe_keys[i])]) *
             static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}
