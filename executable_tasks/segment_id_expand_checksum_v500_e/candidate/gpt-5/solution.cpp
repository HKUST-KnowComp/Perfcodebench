#include "interface.h"

#include <vector>

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  static constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  static constexpr uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t hash = FNV_OFFSET_BASIS;

  const std::size_t n = offsets.size();
  if (n <= 1) {
    // No segments to expand; hash remains the offset basis.
    return hash;
  }

  const uint32_t* data = offsets.data();
  uint32_t prev = data[0];
  for (std::size_t s = 0; s + 1 < n; ++s) {
    const uint32_t next = data[s + 1];
    uint32_t len = next - prev;
    prev = next;

    if (len == 0) continue;

    const uint64_t v = static_cast<uint64_t>(s);

    // Unrolled processing for better throughput on long runs
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
