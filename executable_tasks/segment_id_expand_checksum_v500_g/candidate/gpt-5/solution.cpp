#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  // Preserve baseline behavior for iters == 0 (returns 0)
  if (iters <= 0) return 0ULL;

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t hash = FNV_OFFSET;

  const std::size_t n = offsets.size();
  if (n >= 2) {
    const uint32_t* ptr = offsets.data();
    const std::size_t segs = n - 1;

    for (std::size_t s = 0; s < segs; ++s) {
      uint32_t len = ptr[s + 1] - ptr[s];  // Preserve uint32_t wrap semantics
      const uint64_t v = static_cast<uint64_t>(s);

      // Unroll by 8 for throughput
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
      // Remainder (at most 7 iterations)
      while (len--) {
        hash ^= v;
        hash *= FNV_PRIME;
      }
    }
  }

  return hash;
}
