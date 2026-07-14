#include "interface.h"

#include <cstddef>
#include <cstdint>

static inline std::size_t lower_bound_index(const uint32_t* data, std::size_t n, uint32_t key) {
  std::size_t first = 0;
  while (n > 0) {
    std::size_t half = n >> 1;
    std::size_t mid = first + half;
    if (data[mid] < key) {
      first = mid + 1;
      n -= half + 1;
    } else {
      n = half;
    }
  }
  return first;
}

uint64_t join_sum(
    const std::vector<uint32_t>& dim_keys,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
  const std::size_t n_probes = probe_keys.size();

  // Handle degenerate cases defensively.
  if (dim_keys.empty() || dim_values.empty() || n_probes == 0) {
    return 0ull;
  }

  const uint32_t* dk = dim_keys.data();
  const uint32_t* dv = dim_values.data();
  const uint32_t* pk = probe_keys.data();
  const uint32_t* pw = probe_weights.data();
  const std::size_t dim_n = dim_keys.size();

  const uint32_t base = dk[0];
  const uint32_t last = dk[dim_n - 1];
  const bool dense = (static_cast<uint64_t>(last) - static_cast<uint64_t>(base) + 1ull) == static_cast<uint64_t>(dim_n);

  uint64_t sum = 0;
  if (dense) {
    // Fast path: contiguous keys, direct indexing by offset from base.
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t s = 0;
      std::size_t i = 0;
      const std::size_t n = n_probes;
      // Unroll by 4 for better throughput
      for (; i + 3 < n; i += 4) {
        const std::size_t idx0 = static_cast<std::size_t>(pk[i + 0] - base);
        const std::size_t idx1 = static_cast<std::size_t>(pk[i + 1] - base);
        const std::size_t idx2 = static_cast<std::size_t>(pk[i + 2] - base);
        const std::size_t idx3 = static_cast<std::size_t>(pk[i + 3] - base);
        s += static_cast<uint64_t>(dv[idx0]) * static_cast<uint64_t>(pw[i + 0]);
        s += static_cast<uint64_t>(dv[idx1]) * static_cast<uint64_t>(pw[i + 1]);
        s += static_cast<uint64_t>(dv[idx2]) * static_cast<uint64_t>(pw[i + 2]);
        s += static_cast<uint64_t>(dv[idx3]) * static_cast<uint64_t>(pw[i + 3]);
      }
      for (; i < n; ++i) {
        const std::size_t idx = static_cast<std::size_t>(pk[i] - base);
        s += static_cast<uint64_t>(dv[idx]) * static_cast<uint64_t>(pw[i]);
      }
      sum = s;
    }
  } else {
    // Fallback: use lower_bound equivalent to find index.
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t s = 0;
      for (std::size_t i = 0; i < n_probes; ++i) {
        const std::size_t idx = lower_bound_index(dk, dim_n, pk[i]);
        s += static_cast<uint64_t>(dv[idx]) * static_cast<uint64_t>(pw[i]);
      }
      sum = s;
    }
  }

  return sum;
}
