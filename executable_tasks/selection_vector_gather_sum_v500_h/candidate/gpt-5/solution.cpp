#include "interface.h"

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int /*iters*/) {
  const std::size_t n = keys.size();
  if (n == 0) {
    return 0ull;
  }

  const uint32_t* kptr = keys.data();
  const uint8_t* fptr = flags.data();
  const uint32_t* pptr = payloads.data();
  const uint32_t t = threshold;

  uint64_t sum0 = 0;
  uint64_t sum1 = 0;

  std::size_t i = 0;
  // Unroll by 4 for better ILP and reduced branch overhead
  for (; i + 3 < n; i += 4) {
    uint32_t k0 = kptr[i];
    uint32_t k1 = kptr[i + 1];
    uint32_t k2 = kptr[i + 2];
    uint32_t k3 = kptr[i + 3];

    if (fptr[i] && k0 < t && ((k0 & 7u) == 0u)) {
      sum0 += static_cast<uint64_t>(pptr[i]);
    }
    if (fptr[i + 1] && k1 < t && ((k1 & 7u) == 0u)) {
      sum0 += static_cast<uint64_t>(pptr[i + 1]);
    }
    if (fptr[i + 2] && k2 < t && ((k2 & 7u) == 0u)) {
      sum1 += static_cast<uint64_t>(pptr[i + 2]);
    }
    if (fptr[i + 3] && k3 < t && ((k3 & 7u) == 0u)) {
      sum1 += static_cast<uint64_t>(pptr[i + 3]);
    }
  }

  // Handle remaining elements
  for (; i < n; ++i) {
    uint32_t k = kptr[i];
    if (fptr[i] && k < t && ((k & 7u) == 0u)) {
      sum0 += static_cast<uint64_t>(pptr[i]);
    }
  }

  return sum0 + sum1;
}
