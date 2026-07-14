#include "interface.h"

#include <vector>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  if (dim_keys.empty() || probe_keys.empty()) {
    return 0;
  }

  // dim_keys is sorted per baseline's lower_bound requirement, so min/max are first/last elements
  const uint32_t min_key = dim_keys[0];
  const uint32_t max_key = dim_keys.back();
  const size_t lookup_size = max_key - min_key + 1;
  std::vector<uint32_t> lookup(lookup_size, 0);

  // Populate lookup once before iteration loop to avoid redundant work
  for (size_t i = 0; i < dim_keys.size(); ++i) {
    const size_t offset = dim_keys[i] - min_key;
    lookup[offset] = dim_values[i];
  }

  uint64_t final_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    const size_t probe_len = probe_keys.size();
    // Linear scan of probe data with constant-time lookups, compiler-friendly for vectorization
    for (size_t i = 0; i < probe_len; ++i) {
      const size_t offset = probe_keys[i] - min_key;
      sum += static_cast<uint64_t>(lookup[offset]) * static_cast<uint64_t>(probe_weights[i]);
    }
    final_sum = sum;
  }
  return final_sum;
}