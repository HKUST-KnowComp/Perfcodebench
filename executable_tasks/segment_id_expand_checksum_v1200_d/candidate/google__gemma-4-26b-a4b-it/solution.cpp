#include "interface.h"
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (offsets.size() < 2) {
    // If there are no segments to expand, the loop logic implies 
    // the hash is reset to the FNV offset basis each iteration.
    // However, if no elements are processed, the hash remains the basis.
    // Based on the baseline, if expanded is empty, hash = 1469598103934665603ULL.
    return (iters > 0) ? 1469598103934665603ULL : 0;
  }

  uint64_t hash = 0;
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET_BASIS;
    for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
      const uint32_t len = offsets[s + 1] - offsets[s];
      const uint64_t val = static_cast<uint64_t>(s);
      
      // Unrolling or simple loop to process the segment
      // The compiler will likely vectorize or unroll this.
      for (uint32_t i = 0; i < len; ++i) {
        hash = mix(hash, val);
      }
    }
  }

  return hash;
}