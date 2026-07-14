#include "interface.h"

#include <algorithm>
#include <vector>
#include <cstddef>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (dim_keys.empty() || probe_keys.empty()) return 0;

  const uint32_t* d_vals = dim_values.data();
  const uint32_t* p_keys = probe_keys.data();
  const uint32_t* p_weights = probe_weights.data();
  const std::size_t n = probe_keys.size();

  const uint32_t min_key = dim_keys.front();
  const uint32_t max_key = dim_keys.back();
  const bool is_dense = (max_key - min_key == dim_keys.size() - 1);

  uint64_t sum = 0;

  if (is_dense) {
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
      std::size_t i = 0;
      for (; i + 3 < n; i += 4) {
        s0 += static_cast<uint64_t>(d_vals[p_keys[i] - min_key]) * p_weights[i];
        s1 += static_cast<uint64_t>(d_vals[p_keys[i+1] - min_key]) * p_weights[i+1];
        s2 += static_cast<uint64_t>(d_vals[p_keys[i+2] - min_key]) * p_weights[i+2];
        s3 += static_cast<uint64_t>(d_vals[p_keys[i+3] - min_key]) * p_weights[i+3];
      }
      for (; i < n; ++i) {
        s0 += static_cast<uint64_t>(d_vals[p_keys[i] - min_key]) * p_weights[i];
      }
      sum = s0 + s1 + s2 + s3;
    }
  } else {
    for (int iter = 0; iter < iters; ++iter) {
      sum = 0;
      for (std::size_t i = 0; i < n; ++i) {
        const auto it = std::lower_bound(dim_keys.begin(), dim_keys.end(), p_keys[i]);
        const std::size_t idx = static_cast<std::size_t>(it - dim_keys.begin());
        sum += static_cast<uint64_t>(d_vals[idx]) * static_cast<uint64_t>(p_weights[i]);
      }
    }
  }
  return sum;
}
