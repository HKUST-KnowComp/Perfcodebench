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
  uint64_t hash = 0;
  const std::size_t n = offsets.size();

  if (iters <= 0) return hash;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;

    if (n >= 2) {
      for (std::size_t s = 0; s + 1 < n; ++s) {
        const uint32_t len = offsets[s + 1] - offsets[s];
        const uint64_t v = static_cast<uint32_t>(s);

        uint32_t i = 0;
        const uint32_t unroll = len & ~uint32_t(7);
        for (; i < unroll; i += 8) {
          h = mix(h, v);
          h = mix(h, v);
          h = mix(h, v);
          h = mix(h, v);
          h = mix(h, v);
          h = mix(h, v);
          h = mix(h, v);
          h = mix(h, v);
        }
        for (; i < len; ++i) {
          h = mix(h, v);
        }
      }
    }

    hash = h;
  }

  return hash;
}
