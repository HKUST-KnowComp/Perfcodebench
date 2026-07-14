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
    // If there are no segments to expand, the loop logic implies the hash
    // is reset to the FNV offset basis in each iteration.
    // However, if iters > 0, the last iteration's hash is returned.
    // Based on the baseline, if the inner loop doesn't run, hash becomes the basis.
    return (iters > 0) ? 1469598103934665603ULL : 0;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
      const uint32_t len = offsets[s + 1] - offsets[s];
      const uint64_t segment_id = static_cast<uint64_t>(s);
      
      // The baseline performs: for (i=0; i<len; ++i) hash = mix(hash, s);
      // We can optimize this by observing that we are applying the same 
      // transformation 'len' times.
      // However, since the mix function is not easily reducible via power-of-two 
      // exponentiation in a simple way without careful math, and 'len' is likely 
      // small enough or the bottleneck is memory, we unroll or just loop.
      // Given the constraints, a direct loop is much faster than vector allocation.
      for (uint32_t i = 0; i < len; ++i) {
        hash = mix(hash, segment_id);
      }
    }
  }
  return hash;
}