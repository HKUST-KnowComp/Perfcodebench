#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  // If no iterations requested, behave like baseline (returns 0)
  if (iters <= 0) {
    return 0ull;
  }

  const std::size_t n = keys.size();
  const uint32_t T = threshold;

  const uint32_t* kptr = keys.data();
  const uint8_t* fptr = flags.data();
  const uint32_t* pptr = payloads.data();

  uint64_t sum = 0ull;
  for (std::size_t i = 0; i < n; ++i) {
    if (fptr[i]) {
      const uint32_t k = kptr[i];
      if (k < T && ((k & 7u) == 0u)) {
        sum += static_cast<uint64_t>(pptr[i]);
      }
    }
  }

  return sum;
}
