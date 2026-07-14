#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  // Preserve baseline semantics: if iters <= 0, return 0.
  if (iters <= 0) {
    return 0ull;
  }

  // If threshold is zero, nothing can pass the predicate.
  if (threshold == 0u) {
    return 0ull;
  }

  // Iterate up to the minimum of the input sizes to avoid out-of-bounds
  // (cheap and done once).
  std::size_t n = keys.size();
  std::size_t nf = flags.size();
  std::size_t np = payloads.size();
  if (nf < n) n = nf;
  if (np < n) n = np;
  if (n == 0) {
    return 0ull;
  }

  const uint32_t* k = keys.data();
  const uint8_t* f = flags.data();
  const uint32_t* p = payloads.data();
  const uint32_t* kend = k + n;

  uint64_t sum = 0ull;

  // Unroll by 4 to reduce loop overhead while keeping cache-friendly access.
  while (k + 4 <= kend) {
    uint32_t k0 = k[0]; uint8_t f0 = f[0];
    if (f0 && k0 < threshold && ((k0 & 7u) == 0u)) {
      sum += static_cast<uint64_t>(p[0]);
    }
    uint32_t k1 = k[1]; uint8_t f1 = f[1];
    if (f1 && k1 < threshold && ((k1 & 7u) == 0u)) {
      sum += static_cast<uint64_t>(p[1]);
    }
    uint32_t k2 = k[2]; uint8_t f2 = f[2];
    if (f2 && k2 < threshold && ((k2 & 7u) == 0u)) {
      sum += static_cast<uint64_t>(p[2]);
    }
    uint32_t k3 = k[3]; uint8_t f3 = f[3];
    if (f3 && k3 < threshold && ((k3 & 7u) == 0u)) {
      sum += static_cast<uint64_t>(p[3]);
    }
    k += 4; f += 4; p += 4;
  }

  // Handle remaining elements.
  while (k < kend) {
    uint32_t ki = *k;
    if (*f && ki < threshold && ((ki & 7u) == 0u)) {
      sum += static_cast<uint64_t>(*p);
    }
    ++k; ++f; ++p;
  }

  return sum;
}
