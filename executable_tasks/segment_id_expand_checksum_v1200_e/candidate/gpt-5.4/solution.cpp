#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  const std::size_t n = offsets.size();
  uint64_t hash = 0;

  if (iters <= 0) return hash;
  if (n < 2) {
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;

    for (std::size_t s = 0; s + 1 < n; ++s) {
      uint32_t len = offsets[s + 1] - offsets[s];
      const uint64_t v = static_cast<uint32_t>(s);

      while (len >= 8) {
        h = mix(h, v);
        h = mix(h, v);
        h = mix(h, v);
        h = mix(h, v);
        h = mix(h, v);
        h = mix(h, v);
        h = mix(h, v);
        h = mix(h, v);
        len -= 8;
      }
      while (len--) {
        h = mix(h, v);
      }
    }

    hash = h;
  }

  return hash;
}
