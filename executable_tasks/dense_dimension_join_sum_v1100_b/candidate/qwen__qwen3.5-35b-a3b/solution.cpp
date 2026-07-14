#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  if (dim_keys.empty()) {
    return 0;
  }
  uint32_t offset = dim_keys[0];
  const uint32_t* dim_values_ptr = dim_values.data();
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < probe_keys.size(); ++i) {
      std::size_t idx = probe_keys[i] - offset;
      sum += static_cast<uint64_t>(dim_values_ptr[idx]) * static_cast<uint64_t>(probe_weights[i]);
    }
  }
  return sum;
}