#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (dim_keys.empty() || dim_values.empty()) {
    return 0;
  }
  const uint32_t base = dim_keys[0];
  const uint32_t* const dvals = dim_values.data();
  const uint32_t* const pkeys = probe_keys.data();
  const uint32_t* const pw = probe_weights.data();
  const std::size_t n = probe_keys.size();
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      const std::size_t idx = static_cast<std::size_t>(pkeys[i] - base);
      sum += static_cast<uint64_t>(dvals[idx]) * static_cast<uint64_t>(pw[i]);
    }
  }
  return sum;
}