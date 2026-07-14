#include "interface.h"
#include <vector>
#include <cstddef>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  if (iters <= 0) return 0;
  const uint8_t* f = flags.data();
  const uint32_t* k = keys.data();
  const uint32_t* p = payloads.data();
  std::size_t n = keys.size();
  uint64_t sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    if (f[i] && k[i] < threshold && (k[i] & 7u) == 0u) {
      sum += p[i];
    }
  }
  return sum;
}