#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  uint64_t sum = 0;
  const uint32_t offset = dim_keys[0];
  const uint32_t* dv = dim_values.data();
  const uint32_t* pk = probe_keys.data();
  const uint32_t* pw = probe_weights.data();
  const size_t n = probe_keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (size_t i = 0; i < n; ++i) {
      iter_sum += static_cast<uint64_t>(dv[pk[i] - offset]) * pw[i];
    }
    sum = iter_sum;
  }
  return sum;
}