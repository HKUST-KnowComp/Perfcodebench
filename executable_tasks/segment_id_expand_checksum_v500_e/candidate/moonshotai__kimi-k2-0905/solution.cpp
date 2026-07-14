#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  const std::size_t n = offsets.size();
  if (n < 2) return 0;

  const uint64_t init = 1469598103934665603ULL;
  uint64_t hash = init;

  for (int iter = 0; iter < iters; ++iter) {
    hash = init;
    for (std::size_t s = 0; s + 1 < n; ++s) {
      const uint32_t seg_id = static_cast<uint32_t>(s);
      uint32_t len = offsets[s + 1] - offsets[s];

      // Unroll by 4
      while (len >= 4) {
        hash = mix(hash, seg_id);
        hash = mix(hash, seg_id);
        hash = mix(hash, seg_id);
        hash = mix(hash, seg_id);
        len -= 4;
      }
      // Tail
      while (len--) {
        hash = mix(hash, seg_id);
      }
    }
  }
  return hash;
}