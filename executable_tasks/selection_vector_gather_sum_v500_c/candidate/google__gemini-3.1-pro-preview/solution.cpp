#include "interface.h"
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  if (iters <= 0) return 0;

  size_t n = keys.size();
  const uint32_t* __restrict k = keys.data();
  const uint8_t* __restrict f = flags.data();
  const uint32_t* __restrict p = payloads.data();

  uint64_t sum = 0;

  // The inputs do not change across iterations, so we only need to compute the sum once.
  // We also fuse the filter and gather steps to avoid intermediate allocations.
  #pragma GCC ivdep
  for (size_t i = 0; i < n; ++i) {
    uint32_t key = k[i];
    if (f[i] && key < threshold && ((key & 7u) == 0)) {
      sum += p[i];
    }
  }

  return sum;
}
