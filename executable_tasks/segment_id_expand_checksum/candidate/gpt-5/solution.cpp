#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline void mix_repeat(uint64_t& h, uint64_t v, uint32_t count) {
  const uint64_t P = 1099511628211ULL;
  while (count >= 8) {
    h ^= v; h *= P;
    h ^= v; h *= P;
    h ^= v; h *= P;
    h ^= v; h *= P;
    h ^= v; h *= P;
    h ^= v; h *= P;
    h ^= v; h *= P;
    h ^= v; h *= P;
    count -= 8;
  }
  switch (count) {
    case 7: h ^= v; h *= P; [[fallthrough]];
    case 6: h ^= v; h *= P; [[fallthrough]];
    case 5: h ^= v; h *= P; [[fallthrough]];
    case 4: h ^= v; h *= P; [[fallthrough]];
    case 3: h ^= v; h *= P; [[fallthrough]];
    case 2: h ^= v; h *= P; [[fallthrough]];
    case 1: h ^= v; h *= P; [[fallthrough]];
    default: break;
  }
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters <= 0) return 0ULL;

  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis

  const std::size_t n = offsets.size();
  if (n >= 2) {
    const uint32_t* off = offsets.data();
    for (std::size_t s = 0; s + 1 < n; ++s) {
      const uint32_t len = off[s + 1] - off[s];
      if (len == 0) continue;
      mix_repeat(hash, static_cast<uint64_t>(s), len);
    }
  }

  // Subsequent iterations would produce the same hash; return it directly.
  return hash;
}
