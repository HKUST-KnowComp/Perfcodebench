#include "interface.h"

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  (void)iters;
  uint64_t sum = 0;
  const size_t n = keys.size();
  const uint8_t* f = flags.data();
  const uint32_t* k = keys.data();
  const uint32_t* p = payloads.data();
  for (size_t i = 0; i < n; ++i) {
    if (f[i] && k[i] < threshold && !(k[i] & 7)) {
      sum += p[i];
    }
  }
  return sum;
}
