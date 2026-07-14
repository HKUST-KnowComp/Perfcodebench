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

  if (iters <= 0 || probe_n == 0 || dim_n == 0) return 0;

  // Dense-dimension fast path: build a direct lookup table once, then probe by key.
  // The task guarantees a dense dimension, and the baseline assumes every probe key
  // exists in the dimension. We preserve that behavior while avoiding per-probe
  // binary searches.
  uint32_t min_key = dim_keys[0];
  uint32_t max_key = dim_keys[0];
  for (std::size_t i = 1; i < dim_n; ++i) {
    const uint32_t k = dim_keys[i];
    if (k < min_key) min_key = k;
    if (k > max_key) max_key = k;
  }

  const uint64_t range64 = static_cast<uint64_t>(max_key) - static_cast<uint64_t>(min_key) + 1u;
  std::vector<uint32_t> lut(static_cast<std::size_t>(range64));
  for (std::size_t i = 0; i < dim_n; ++i) {
    lut[static_cast<std::size_t>(dim_keys[i] - min_key)] = dim_values[i];
  }

  const uint32_t* __restrict probe_k = probe_keys.data();
  const uint32_t* __restrict probe_w = probe_weights.data();
  const uint32_t* __restrict table = lut.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t acc0 = 0;
    uint64_t acc1 = 0;
    uint64_t acc2 = 0;
    uint64_t acc3 = 0;

    std::size_t i = 0;
    const std::size_t unroll_end = probe_n & ~std::size_t(3);
    for (; i < unroll_end; i += 4) {
      const std::size_t idx0 = static_cast<std::size_t>(probe_k[i] - min_key);
      const std::size_t idx1 = static_cast<std::size_t>(probe_k[i + 1] - min_key);
      const std::size_t idx2 = static_cast<std::size_t>(probe_k[i + 2] - min_key);
      const std::size_t idx3 = static_cast<std::size_t>(probe_k[i + 3] - min_key);

      acc0 += static_cast<uint64_t>(table[idx0]) * static_cast<uint64_t>(probe_w[i]);
      acc1 += static_cast<uint64_t>(table[idx1]) * static_cast<uint64_t>(probe_w[i + 1]);
      acc2 += static_cast<uint64_t>(table[idx2]) * static_cast<uint64_t>(probe_w[i + 2]);
      acc3 += static_cast<uint64_t>(table[idx3]) * static_cast<uint64_t>(probe_w[i + 3]);
    }

    uint64_t tail = 0;
    for (; i < probe_n; ++i) {
      const std::size_t idx = static_cast<std::size_t>(probe_k[i] - min_key);
      tail += static_cast<uint64_t>(table[idx]) * static_cast<uint64_t>(probe_w[i]);
    }

    sum = acc0 + acc1 + acc2 + acc3 + tail;
  }

  return sum;
}
