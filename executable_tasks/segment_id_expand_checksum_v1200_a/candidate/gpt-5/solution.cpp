#include "interface.h"

#include <vector>

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  // Preserve baseline behavior: if iters <= 0, hash remains 0 and is returned.
  if (iters <= 0) {
    return 0ULL;
  }

  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t h = FNV_OFFSET_BASIS;

  const std::size_t n = offsets.size();
  for (std::size_t s = 0; s + 1 < n; ++s) {
    const uint32_t len = static_cast<uint32_t>(offsets[s + 1] - offsets[s]);
    const uint64_t v = static_cast<uint64_t>(s);

    uint32_t cnt = len;
    // Unrolled processing for better throughput on repeated identical values.
    while (cnt >= 8) {
      h ^= v; h *= FNV_PRIME;
      h ^= v; h *= FNV_PRIME;
      h ^= v; h *= FNV_PRIME;
      h ^= v; h *= FNV_PRIME;
      h ^= v; h *= FNV_PRIME;
      h ^= v; h *= FNV_PRIME;
      h ^= v; h *= FNV_PRIME;
      h ^= v; h *= FNV_PRIME;
      cnt -= 8;
    }
    switch (cnt) {
      case 7: h ^= v; h *= FNV_PRIME; [[fallthrough]];
      case 6: h ^= v; h *= FNV_PRIME; [[fallthrough]];
      case 5: h ^= v; h *= FNV_PRIME; [[fallthrough]];
      case 4: h ^= v; h *= FNV_PRIME; [[fallthrough]];
      case 3: h ^= v; h *= FNV_PRIME; [[fallthrough]];
      case 2: h ^= v; h *= FNV_PRIME; [[fallthrough]];
      case 1: h ^= v; h *= FNV_PRIME; [[fallthrough]];
      default: break;
    }
  }

  return h;
}
