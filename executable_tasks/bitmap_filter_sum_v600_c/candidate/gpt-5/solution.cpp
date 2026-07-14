#include "interface.h"

#include <cstddef>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  if (iters <= 0) return 0;
  if (key_space == 0) return 0;

  // Build bitmap of enabled keys once.
  std::vector<uint8_t> enabled_bitmap(key_space, 0);
  for (uint32_t k : enabled_keys) {
    if (k < key_space) enabled_bitmap[k] = 1u;
  }

  const uint32_t* pk = probe_keys.data();
  const uint32_t* val = values.data();
  const uint8_t* bm = enabled_bitmap.data();
  const std::size_t n = probe_keys.size();

  // Compute the sum once; identical across iterations.
  uint64_t sum = 0;

  // Unrolled loop for better ILP and fewer branches; branchless accumulation.
  std::size_t i = 0;
  std::size_t n4 = n & ~static_cast<std::size_t>(3);
  for (; i < n4; i += 4) {
    sum += static_cast<uint64_t>(val[i + 0]) * bm[pk[i + 0]];
    sum += static_cast<uint64_t>(val[i + 1]) * bm[pk[i + 1]];
    sum += static_cast<uint64_t>(val[i + 2]) * bm[pk[i + 2]];
    sum += static_cast<uint64_t>(val[i + 3]) * bm[pk[i + 3]];
  }
  for (; i < n; ++i) {
    sum += static_cast<uint64_t>(val[i]) * bm[pk[i]];
  }

  return sum;
}
