#include "interface.h"

#include <algorithm>

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int /*iters*/) {
  const std::size_t n_dim = dim_keys.size();
  const std::size_t n_probe = probe_keys.size();

  if (n_probe == 0 || n_dim == 0) {
    return 0ULL;
  }

  // Check if the dimension keys are contiguous: dim_keys[i] == base + i
  const uint32_t base = dim_keys.front();
  bool contiguous = false;
  if (dim_keys.back() == base + static_cast<uint32_t>(n_dim - 1)) {
    contiguous = true;
    for (std::size_t i = 0; i < n_dim; ++i) {
      if (dim_keys[i] != base + static_cast<uint32_t>(i)) {
        contiguous = false;
        break;
      }
    }
  }

  uint64_t sum = 0ULL;

  if (contiguous) {
    const uint32_t* pk = probe_keys.data();
    const uint32_t* pw = probe_weights.data();
    const uint32_t* dv = dim_values.data();
    const uint32_t off = base;

    // Single pass: direct index into dim_values using key - base
    for (std::size_t i = 0; i < n_probe; ++i) {
      const uint32_t k = pk[i];
      const std::size_t idx = static_cast<std::size_t>(k - off);
      sum += static_cast<uint64_t>(dv[idx]) * static_cast<uint64_t>(pw[i]);
    }
  } else {
    // Fallback: use binary search per probe (pointer-based for speed)
    const uint32_t* dk_begin = dim_keys.data();
    const uint32_t* dk_end = dk_begin + n_dim;
    const uint32_t* dv = dim_values.data();
    const uint32_t* pk = probe_keys.data();
    const uint32_t* pw = probe_weights.data();

    for (std::size_t i = 0; i < n_probe; ++i) {
      const uint32_t key = pk[i];
      const uint32_t* it = std::lower_bound(dk_begin, dk_end, key);
      const std::size_t idx = static_cast<std::size_t>(it - dk_begin);
      sum += static_cast<uint64_t>(dv[idx]) * static_cast<uint64_t>(pw[i]);
    }
  }

  return sum;
}
