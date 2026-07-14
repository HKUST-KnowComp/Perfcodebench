#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  const std::size_t n = keys.size();
  if (n == 0) return 0;

  const uint32_t* __restrict k_ptr = keys.data();
  const uint8_t* __restrict f_ptr = flags.data();
  const uint32_t* __restrict p_ptr = payloads.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;

    // Process 4 elements at a time
    const std::size_t n4 = n & ~std::size_t(3);
    for (; i < n4; i += 4) {
      uint32_t k0 = k_ptr[i];
      uint32_t k1 = k_ptr[i + 1];
      uint32_t k2 = k_ptr[i + 2];
      uint32_t k3 = k_ptr[i + 3];

      // Branchless predicate: flags[i] && keys[i] < threshold && (keys[i] & 7) == 0
      // Use multiplication to combine conditions (0 or 1)
      uint32_t m0 = f_ptr[i] & (k0 < threshold) & ((k0 & 7u) == 0u);
      uint32_t m1 = f_ptr[i + 1] & (k1 < threshold) & ((k1 & 7u) == 0u);
      uint32_t m2 = f_ptr[i + 2] & (k2 < threshold) & ((k2 & 7u) == 0u);
      uint32_t m3 = f_ptr[i + 3] & (k3 < threshold) & ((k3 & 7u) == 0u);

      // Mask payloads and accumulate
      local_sum += static_cast<uint64_t>(p_ptr[i]) * m0;
      local_sum += static_cast<uint64_t>(p_ptr[i + 1]) * m1;
      local_sum += static_cast<uint64_t>(p_ptr[i + 2]) * m2;
      local_sum += static_cast<uint64_t>(p_ptr[i + 3]) * m3;
    }

    // Handle remaining elements
    for (; i < n; ++i) {
      uint32_t k = k_ptr[i];
      uint32_t m = f_ptr[i] & (k < threshold) & ((k & 7u) == 0u);
      local_sum += static_cast<uint64_t>(p_ptr[i]) * m;
    }

    sum = local_sum;
  }

  return sum;
}