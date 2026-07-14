#include "interface.h"

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  // Match baseline behavior: if iters <= 0, return 0; otherwise compute once.
  if (iters <= 0) {
    return 0ULL;
  }

  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t hash = FNV_OFFSET_BASIS;

  const std::size_t n = offsets.size();
  const uint32_t* data = offsets.data();

  for (std::size_t s = 0; s + 1 < n; ++s) {
    // Preserve baseline semantics: subtract as uint32_t (wrap-around if any)
    uint32_t len = static_cast<uint32_t>(data[s + 1] - data[s]);
    // Preserve baseline casting: segment id stored as uint32_t, then hashed as uint64_t
    const uint64_t v = static_cast<uint64_t>(static_cast<uint32_t>(s));

    // Unrolled mixing for performance
    while (len >= 8) {
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      hash ^= v; hash *= FNV_PRIME;
      len -= 8;
    }
    switch (len) {
      case 7: hash ^= v; hash *= FNV_PRIME; [[fallthrough]];
      case 6: hash ^= v; hash *= FNV_PRIME; [[fallthrough]];
      case 5: hash ^= v; hash *= FNV_PRIME; [[fallthrough]];
      case 4: hash ^= v; hash *= FNV_PRIME; [[fallthrough]];
      case 3: hash ^= v; hash *= FNV_PRIME; [[fallthrough]];
      case 2: hash ^= v; hash *= FNV_PRIME; [[fallthrough]];
      case 1: hash ^= v; hash *= FNV_PRIME; [[fallthrough]];
      default: break;
    }
  }

  return hash;
}
