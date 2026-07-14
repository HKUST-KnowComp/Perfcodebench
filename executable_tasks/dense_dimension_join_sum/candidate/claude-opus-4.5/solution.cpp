#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

#if defined(__GNUC__) || defined(__clang__)
#define PREFETCH(addr) __builtin_prefetch(addr, 0, 0)
#else
#define PREFETCH(addr) ((void)0)
#endif

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

  if (dim_keys.empty() || probe_keys.empty()) {
    return 0;
  }

  // Find the range of dimension keys to size the lookup table
  uint32_t min_key = dim_keys[0];
  uint32_t max_key = dim_keys[0];
  for (std::size_t i = 1; i < dim_keys.size(); ++i) {
    if (dim_keys[i] < min_key) min_key = dim_keys[i];
    if (dim_keys[i] > max_key) max_key = dim_keys[i];
  }

  // Build direct-index lookup table: lookup[key - min_key] = value
  const std::size_t table_size = static_cast<std::size_t>(max_key - min_key) + 1;
  std::vector<uint32_t> lookup(table_size, 0);
  for (std::size_t i = 0; i < dim_keys.size(); ++i) {
    lookup[dim_keys[i] - min_key] = dim_values[i];
  }

  const uint32_t* __restrict__ lookup_ptr = lookup.data();
  const uint32_t* __restrict__ pk_ptr = probe_keys.data();
  const uint32_t* __restrict__ pw_ptr = probe_weights.data();
  const std::size_t n = probe_keys.size();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    std::size_t i = 0;
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);

    // Prefetch distance tuned for typical cache line / memory latency
    constexpr std::size_t PF_DIST = 16;

    for (; i < n4; i += 4) {
      // Prefetch future probe data
      if (i + PF_DIST < n) {
        PREFETCH(&pk_ptr[i + PF_DIST]);
        PREFETCH(&pw_ptr[i + PF_DIST]);
      }

      const uint32_t k0 = pk_ptr[i]     - min_key;
      const uint32_t k1 = pk_ptr[i + 1] - min_key;
      const uint32_t k2 = pk_ptr[i + 2] - min_key;
      const uint32_t k3 = pk_ptr[i + 3] - min_key;

      // Prefetch lookup entries
      PREFETCH(&lookup_ptr[k0]);
      PREFETCH(&lookup_ptr[k1]);
      PREFETCH(&lookup_ptr[k2]);
      PREFETCH(&lookup_ptr[k3]);

      s0 += static_cast<uint64_t>(lookup_ptr[k0]) * static_cast<uint64_t>(pw_ptr[i]);
      s1 += static_cast<uint64_t>(lookup_ptr[k1]) * static_cast<uint64_t>(pw_ptr[i + 1]);
      s2 += static_cast<uint64_t>(lookup_ptr[k2]) * static_cast<uint64_t>(pw_ptr[i + 2]);
      s3 += static_cast<uint64_t>(lookup_ptr[k3]) * static_cast<uint64_t>(pw_ptr[i + 3]);
    }

    // Handle remainder
    for (; i < n; ++i) {
      const uint32_t k = pk_ptr[i] - min_key;
      s0 += static_cast<uint64_t>(lookup_ptr[k]) * static_cast<uint64_t>(pw_ptr[i]);
    }

    sum = s0 + s1 + s2 + s3;
  }

  return sum;
}
