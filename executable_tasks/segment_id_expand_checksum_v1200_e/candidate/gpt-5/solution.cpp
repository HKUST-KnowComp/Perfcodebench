#include "interface.h"

namespace {
static inline uint64_t mix_once(uint64_t h, uint64_t v) {
  h ^= v;
  h *= 1099511628211ULL;
  return h;
}
}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters <= 0) return 0;

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t P = 1099511628211ULL;

  uint64_t hash = FNV_OFFSET;

  const std::size_t n = offsets.size();
  if (n >= 2) {
    for (std::size_t s = 0; s + 1 < n; ++s) {
      uint32_t len = offsets[s + 1] - offsets[s];
      if (len == 0) continue;
      const uint64_t v = static_cast<uint64_t>(s);

      while (len >= 8) {
        hash ^= v; hash *= P;
        hash ^= v; hash *= P;
        hash ^= v; hash *= P;
        hash ^= v; hash *= P;
        hash ^= v; hash *= P;
        hash ^= v; hash *= P;
        hash ^= v; hash *= P;
        hash ^= v; hash *= P;
        len -= 8;
      }

      switch (len) {
        case 7: hash ^= v; hash *= P; [[fallthrough]];
        case 6: hash ^= v; hash *= P; [[fallthrough]];
        case 5: hash ^= v; hash *= P; [[fallthrough]];
        case 4: hash ^= v; hash *= P; [[fallthrough]];
        case 3: hash ^= v; hash *= P; [[fallthrough]];
        case 2: hash ^= v; hash *= P; [[fallthrough]];
        case 1: hash ^= v; hash *= P; [[fallthrough]];
        default: break;
      }
    }
  }

  return hash;
}
