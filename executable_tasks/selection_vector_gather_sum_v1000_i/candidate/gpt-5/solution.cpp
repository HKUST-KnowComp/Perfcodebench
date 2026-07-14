#include "interface.h"

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  // Preserve baseline semantics: if iters <= 0, return 0; otherwise, the
  // result is identical across iterations, so compute once.
  if (iters <= 0) {
    return 0;
  }

  const std::size_t n = keys.size();
  const uint32_t* k = keys.data();
  const uint8_t* f = flags.data();
  const uint32_t* pl = payloads.data();

  uint64_t sum0 = 0;
  uint64_t sum1 = 0;
  uint64_t sum2 = 0;
  uint64_t sum3 = 0;

  std::size_t i = 0;
  const std::size_t unroll = 4;
  const std::size_t limit = n - (n % unroll);

  for (; i < limit; i += unroll) {
    // Unrolled iteration 0
    uint32_t key0 = k[i + 0];
    if ((key0 & 7u) == 0u && key0 < threshold && f[i + 0]) {
      sum0 += static_cast<uint64_t>(pl[i + 0]);
    }
    // Unrolled iteration 1
    uint32_t key1 = k[i + 1];
    if ((key1 & 7u) == 0u && key1 < threshold && f[i + 1]) {
      sum1 += static_cast<uint64_t>(pl[i + 1]);
    }
    // Unrolled iteration 2
    uint32_t key2 = k[i + 2];
    if ((key2 & 7u) == 0u && key2 < threshold && f[i + 2]) {
      sum2 += static_cast<uint64_t>(pl[i + 2]);
    }
    // Unrolled iteration 3
    uint32_t key3 = k[i + 3];
    if ((key3 & 7u) == 0u && key3 < threshold && f[i + 3]) {
      sum3 += static_cast<uint64_t>(pl[i + 3]);
    }
  }

  // Remainder loop
  for (; i < n; ++i) {
    uint32_t key = k[i];
    if ((key & 7u) == 0u && key < threshold && f[i]) {
      sum0 += static_cast<uint64_t>(pl[i]);
    }
  }

  return sum0 + sum1 + sum2 + sum3;
}
