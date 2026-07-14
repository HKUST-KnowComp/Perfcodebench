#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t dim_n = dim_keys.size();
  const std::size_t probe_n = probe_keys.size();

  if (iters <= 0 || probe_n == 0 || dim_n == 0) {
    return 0;
  }

  uint32_t max_key = 0;
  for (std::size_t i = 0; i < dim_n; ++i) {
    if (dim_keys[i] > max_key) max_key = dim_keys[i];
  }

  std::vector<uint32_t> lookup(static_cast<std::size_t>(max_key) + 1);
  for (std::size_t i = 0; i < dim_n; ++i) {
    lookup[dim_keys[i]] = dim_values[i];
  }

  const uint32_t* __restrict pk = probe_keys.data();
  const uint32_t* __restrict pw = probe_weights.data();
  const uint32_t* __restrict lv = lookup.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;

    for (; i + 3 < probe_n; i += 4) {
      local_sum += static_cast<uint64_t>(lv[pk[i]]) * static_cast<uint64_t>(pw[i]);
      local_sum += static_cast<uint64_t>(lv[pk[i + 1]]) * static_cast<uint64_t>(pw[i + 1]);
      local_sum += static_cast<uint64_t>(lv[pk[i + 2]]) * static_cast<uint64_t>(pw[i + 2]);
      local_sum += static_cast<uint64_t>(lv[pk[i + 3]]) * static_cast<uint64_t>(pw[i + 3]);
    }
    for (; i < probe_n; ++i) {
      local_sum += static_cast<uint64_t>(lv[pk[i]]) * static_cast<uint64_t>(pw[i]);
    }

    sum = local_sum;
  }

  return sum;
}
