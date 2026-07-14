#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  uint64_t sum = 0;
  const std::size_t n = keys.size();
  const uint32_t* k = keys.data();
  const uint8_t* f = flags.data();
  const uint32_t* p = payloads.data();
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    // Check inexpensive/selective predicates first to avoid unnecessary loads
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t ki = k[i];
      if ((ki & 7u) == 0u && ki < threshold) {
        if (f[i]) {
          s += static_cast<uint64_t>(p[i]);
        }
      }
    }
    sum = s;
  }
  return sum;
}
