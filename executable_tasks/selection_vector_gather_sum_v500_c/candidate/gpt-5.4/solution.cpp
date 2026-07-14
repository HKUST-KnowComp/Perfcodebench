#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  if (iters <= 0) return 0;

  const std::size_t n = keys.size();
  if (n == 0) return 0;

  const uint32_t* k = keys.data();
  const uint8_t* f = flags.data();
  const uint32_t* p = payloads.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t key = k[i];
      if (f[i] && key < threshold && ((key & 7u) == 0u)) {
        local_sum += static_cast<uint64_t>(p[i]);
      }
    }
    sum = local_sum;
  }
  return sum;
}
