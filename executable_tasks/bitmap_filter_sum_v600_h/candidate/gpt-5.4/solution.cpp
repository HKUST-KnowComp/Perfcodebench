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
  // Using bytes instead of bits keeps membership checks very cheap.
  std::vector<uint8_t> enabled(static_cast<std::size_t>(key_space), 0);

  for (uint32_t k : enabled_keys) {
    if (k < key_space) enabled[k] = 1;
  }

  const uint32_t* pk = probe_keys.data();
  const uint32_t* pv = values.data();
  const uint8_t* bm = enabled.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local = 0;

    std::size_t i = 0;
    const std::size_t unroll = 4;
    const std::size_t limit = n & ~(unroll - 1);

    for (; i < limit; i += unroll) {
      const uint32_t k0 = pk[i + 0];
      const uint32_t k1 = pk[i + 1];
      const uint32_t k2 = pk[i + 2];
      const uint32_t k3 = pk[i + 3];

      local += static_cast<uint64_t>(pv[i + 0]) * static_cast<uint64_t>(k0 < key_space && bm[k0]);
      local += static_cast<uint64_t>(pv[i + 1]) * static_cast<uint64_t>(k1 < key_space && bm[k1]);
      local += static_cast<uint64_t>(pv[i + 2]) * static_cast<uint64_t>(k2 < key_space && bm[k2]);
      local += static_cast<uint64_t>(pv[i + 3]) * static_cast<uint64_t>(k3 < key_space && bm[k3]);
    }

    for (; i < n; ++i) {
      const uint32_t k = pk[i];
      if (k < key_space && bm[k]) {
        local += static_cast<uint64_t>(pv[i]);
      }
    }

    sum = local;
  }

  return sum;
}
