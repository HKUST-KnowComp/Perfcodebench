#include "interface.h"

#include <vector>

uint64_t window_sum_checksum(const std::vector<uint32_t>& values, int window, int iters) {
  // Preserve baseline behavior: if iters <= 0, return 0.
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t n = values.size();
  const std::size_t w = static_cast<std::size_t>(window);

  // FNV-1a 64-bit initialization and prime
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t s = 0ULL;

  // First part: i < min(w, n)
  std::size_t limit = (w < n) ? w : n;
  for (std::size_t i = 0; i < limit; ++i) {
    s += static_cast<uint64_t>(values[i]);
    uint64_t v = s;
    hash ^= v;
    hash *= FNV_PRIME;
  }

  // Second part: i >= w (handles w == 0 correctly)
  for (std::size_t i = limit; i < n; ++i) {
    s += static_cast<uint64_t>(values[i]);
    s -= static_cast<uint64_t>(values[i - w]);
    uint64_t v = s;
    hash ^= v;
    hash *= FNV_PRIME;
  }

  return hash;
}
