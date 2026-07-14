#include "interface.h"

#include <vector>

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters <= 0) return 0ULL;

  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t hash = FNV_OFFSET_BASIS;

  const std::size_t n = offsets.size();
  for (std::size_t s = 0; s + 1 < n; ++s) {
    const uint32_t len = offsets[s + 1] - offsets[s];
    const uint64_t v = static_cast<uint32_t>(s);

    uint32_t cnt = len;
    while (cnt >= 8) {
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      cnt -= 8;
    }
    if (cnt >= 4) {
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      cnt -= 4;
    }
    while (cnt) {
      hash ^= v; hash *= FNV_PRIME;
      --cnt;
    }
  }

  return hash;
}
