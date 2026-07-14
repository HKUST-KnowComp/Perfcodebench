#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  // Determine safe iteration bound in case of size mismatches.
  std::size_t n = keys.size();
  if (flags.size() < n) n = flags.size();
  if (payloads.size() < n) n = payloads.size();

  const uint32_t* kptr = keys.data();
  const uint8_t* fptr = flags.data();
  const uint32_t* pptr = payloads.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    std::size_t i = 0;
    const std::size_t n4 = n & ~static_cast<std::size_t>(3);

    for (; i < n4; i += 4) {
      uint32_t k0 = kptr[i];
      if ((fptr[i] != 0) & (k0 < threshold) & ((k0 & 7u) == 0u)) {
        s += static_cast<uint64_t>(pptr[i]);
      }
      uint32_t k1 = kptr[i + 1];
      if ((fptr[i + 1] != 0) & (k1 < threshold) & ((k1 & 7u) == 0u)) {
        s += static_cast<uint64_t>(pptr[i + 1]);
      }
      uint32_t k2 = kptr[i + 2];
      if ((fptr[i + 2] != 0) & (k2 < threshold) & ((k2 & 7u) == 0u)) {
        s += static_cast<uint64_t>(pptr[i + 2]);
      }
      uint32_t k3 = kptr[i + 3];
      if ((fptr[i + 3] != 0) & (k3 < threshold) & ((k3 & 7u) == 0u)) {
        s += static_cast<uint64_t>(pptr[i + 3]);
      }
    }

    for (; i < n; ++i) {
      uint32_t k = kptr[i];
      if ((fptr[i] != 0) & (k < threshold) & ((k & 7u) == 0u)) {
        s += static_cast<uint64_t>(pptr[i]);
      }
    }

    sum = s;
  }

  return sum;
}
