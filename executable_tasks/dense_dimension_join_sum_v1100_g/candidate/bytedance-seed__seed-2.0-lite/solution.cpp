#include "interface.h"
#include <cstddef>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (dim_keys.empty() || probe_keys.empty()) {
    return 0;
  }
  const uint32_t dim_base = dim_keys[0];
  const std::size_t num_probes = probe_keys.size();
  uint64_t final_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    for (std::size_t i = 0; i < num_probes; ++i) {
      const std::size_t idx = static_cast<std::size_t>(probe_keys[i] - dim_base);
      sum += static_cast<uint64_t>(dim_values[idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
    final_sum = sum;
  }
  return final_sum;
}