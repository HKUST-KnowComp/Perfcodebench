#include "interface.h"

#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  const std::size_t n = keys.size();
  const uint32_t* kptr = keys.data();
  const uint8_t* fptr = flags.data();
  const uint32_t* pptr = payloads.data();
  const uint32_t t = threshold;

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s = 0;
    std::size_t i = 0;
    const std::size_t n4 = n & static_cast<std::size_t>(~3ULL);

    // Unrolled loop by 4 to reduce loop overhead
    for (; i < n4; i += 4) {
      uint32_t k0 = kptr[i];
      if (((k0 & 7u) == 0u) && (k0 < t) && fptr[i]) {
        s += static_cast<uint64_t>(pptr[i]);
      }
      uint32_t k1 = kptr[i + 1];
      if (((k1 & 7u) == 0u) && (k1 < t) && fptr[i + 1]) {
        s += static_cast<uint64_t>(pptr[i + 1]);
      }
      uint32_t k2 = kptr[i + 2];
      if (((k2 & 7u) == 0u) && (k2 < t) && fptr[i + 2]) {
        s += static_cast<uint64_t>(pptr[i + 2]);
      }
      uint32_t k3 = kptr[i + 3];
      if (((k3 & 7u) == 0u) && (k3 < t) && fptr[i + 3]) {
        s += static_cast<uint64_t>(pptr[i + 3]);
      }
    }

    // Remainder
    for (; i < n; ++i) {
      uint32_t k = kptr[i];
      if (((k & 7u) == 0u) && (k < t) && fptr[i]) {
        s += static_cast<uint64_t>(pptr[i]);
      }
    }

    sum = s;
  }

  return sum;
}
