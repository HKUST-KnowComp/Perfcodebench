#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  const std::size_t n = probe_keys.size();
  if (iters <= 0 || n == 0 || enabled_keys.empty() || key_space == 0) {
    return 0;
  }

  // Dense bitmap over the bounded key space.
  const std::size_t words = (static_cast<std::size_t>(key_space) + 63u) >> 6;
  std::vector<uint64_t> bitmap(words, 0);

  // Build once: enabled_keys is invariant across iterations.
  for (uint32_t k : enabled_keys) {
    if (k < key_space) {
      bitmap[static_cast<std::size_t>(k) >> 6] |=
          (uint64_t{1} << (k & 63u));
    }
  }

  const uint32_t* pk = probe_keys.data();
  const uint32_t* pv = values.data();
  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t i = 0;
    const std::size_t unroll_end = n & ~std::size_t(3);
    for (; i < unroll_end; i += 4) {
      const uint32_t k0 = pk[i + 0];
      const uint32_t k1 = pk[i + 1];
      const uint32_t k2 = pk[i + 2];
      const uint32_t k3 = pk[i + 3];

      if (k0 < key_space &&
          (bitmap[static_cast<std::size_t>(k0) >> 6] & (uint64_t{1} << (k0 & 63u)))) {
        local_sum += static_cast<uint64_t>(pv[i + 0]);
      }
      if (k1 < key_space &&
          (bitmap[static_cast<std::size_t>(k1) >> 6] & (uint64_t{1} << (k1 & 63u)))) {
        local_sum += static_cast<uint64_t>(pv[i + 1]);
      }
      if (k2 < key_space &&
          (bitmap[static_cast<std::size_t>(k2) >> 6] & (uint64_t{1} << (k2 & 63u)))) {
        local_sum += static_cast<uint64_t>(pv[i + 2]);
      }
      if (k3 < key_space &&
          (bitmap[static_cast<std::size_t>(k3) >> 6] & (uint64_t{1} << (k3 & 63u)))) {
        local_sum += static_cast<uint64_t>(pv[i + 3]);
      }
    }

    for (; i < n; ++i) {
      const uint32_t k = pk[i];
      if (k < key_space &&
          (bitmap[static_cast<std::size_t>(k) >> 6] & (uint64_t{1} << (k & 63u)))) {
        local_sum += static_cast<uint64_t>(pv[i]);
      }
    }

    sum = local_sum;
  }

  return sum;
}
