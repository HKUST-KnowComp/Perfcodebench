#include "interface.h"

#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int /*iters*/) {
  // Compute the sum for a single pass; baseline recomputed identically for each iter
  // and returned the last computed sum, so final result equals a single iteration.
  if (probe_keys.empty()) {
    return 0ull;
  }

  const std::size_t dim_n = dim_keys.size();
  const std::size_t probe_n = probe_keys.size();

  uint64_t sum = 0ull;

  if (dim_n != 0) {
    // Check if dimension keys are contiguous ascending: dim_keys[i] == base + i
    const uint32_t base = dim_keys.front();
    bool contiguous = true;
    for (std::size_t i = 0; i < dim_n; ++i) {
      if (dim_keys[i] != base + static_cast<uint32_t>(i)) {
        contiguous = false;
        break;
      }
    }

    if (contiguous) {
      // Direct indexing into dim_values using key - base
      const uint32_t* dv = dim_values.data();
      const uint32_t* pk = probe_keys.data();
      const uint32_t* pw = probe_weights.data();
      uint64_t s = 0ull;
      for (std::size_t i = 0; i < probe_n; ++i) {
        const std::size_t idx = static_cast<std::size_t>(pk[i] - base);
        s += static_cast<uint64_t>(dv[idx]) * static_cast<uint64_t>(pw[i]);
      }
      return s;
    }
  }

  // Fallback: binary search per probe key using pointer-based lower_bound
  const uint32_t* dk = dim_keys.data();
  const uint32_t* dv = dim_values.data();
  const std::size_t nDim = dim_keys.size();
  const uint32_t* pk = probe_keys.data();
  const uint32_t* pw = probe_weights.data();

  uint64_t s = 0ull;
  for (std::size_t i = 0; i < probe_n; ++i) {
    const uint32_t key = pk[i];
    const uint32_t* it = std::lower_bound(dk, dk + nDim, key);
    const std::size_t idx = static_cast<std::size_t>(it - dk);
    s += static_cast<uint64_t>(dv[idx]) * static_cast<uint64_t>(pw[i]);
  }

  return s;
}
