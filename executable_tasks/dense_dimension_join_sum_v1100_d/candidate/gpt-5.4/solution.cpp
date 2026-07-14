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

  uint32_t min_key = dim_keys[0];
  uint32_t max_key = dim_keys[0];
  for (std::size_t i = 1; i < dim_n; ++i) {
    const uint32_t k = dim_keys[i];
    if (k < min_key) min_key = k;
    if (k > max_key) max_key = k;
  }

  const std::size_t range = static_cast<std::size_t>(max_key - min_key) + 1u;
  std::vector<uint32_t> dense_values(range);
  for (std::size_t i = 0; i < dim_n; ++i) {
    dense_values[static_cast<std::size_t>(dim_keys[i] - min_key)] = dim_values[i];
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;

    for (; i + 3 < probe_n; i += 4) {
      const uint32_t k0 = probe_keys[i + 0];
      const uint32_t k1 = probe_keys[i + 1];
      const uint32_t k2 = probe_keys[i + 2];
      const uint32_t k3 = probe_keys[i + 3];

      local_sum += static_cast<uint64_t>(dense_values[static_cast<std::size_t>(k0 - min_key)]) *
                   static_cast<uint64_t>(probe_weights[i + 0]);
      local_sum += static_cast<uint64_t>(dense_values[static_cast<std::size_t>(k1 - min_key)]) *
                   static_cast<uint64_t>(probe_weights[i + 1]);
      local_sum += static_cast<uint64_t>(dense_values[static_cast<std::size_t>(k2 - min_key)]) *
                   static_cast<uint64_t>(probe_weights[i + 2]);
      local_sum += static_cast<uint64_t>(dense_values[static_cast<std::size_t>(k3 - min_key)]) *
                   static_cast<uint64_t>(probe_weights[i + 3]);
    }

    for (; i < probe_n; ++i) {
      local_sum += static_cast<uint64_t>(
                       dense_values[static_cast<std::size_t>(probe_keys[i] - min_key)]) *
                   static_cast<uint64_t>(probe_weights[i]);
    }

    sum = local_sum;
  }

  return sum;
}
