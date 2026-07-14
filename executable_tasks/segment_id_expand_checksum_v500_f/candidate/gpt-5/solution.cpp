#include "interface.h"

#include <vector>

namespace {
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;
}

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  uint64_t hash = 0ULL;
  const std::size_t n = offsets.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = FNV_OFFSET_BASIS;
    for (std::size_t s = 0; s + 1 < n; ++s) {
      uint32_t len = offsets[s + 1] - offsets[s];
      const uint64_t v = static_cast<uint32_t>(s);

      // Unroll the mix loop for better performance.
      while (len >= 8) {
        h ^= v; h *= FNV_PRIME;
        h ^= v; h *= FNV_PRIME;
        h ^= v; h *= FNV_PRIME;
        h ^= v; h *= FNV_PRIME;
        h ^= v; h *= FNV_PRIME;
        h ^= v; h *= FNV_PRIME;
        h ^= v; h *= FNV_PRIME;
        h ^= v; h *= FNV_PRIME;
        len -= 8;
      }
      while (len > 0) {
        h ^= v;
        h *= FNV_PRIME;
        --len;
      }
    }
    hash = h;
  }

  return hash;
}
