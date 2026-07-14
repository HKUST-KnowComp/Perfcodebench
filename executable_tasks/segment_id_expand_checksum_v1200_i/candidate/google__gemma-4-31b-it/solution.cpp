#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (offsets.size() < 2) {
    // If there are no segments to expand, the loop over offsets won't run.
    // The baseline resets hash to 1469598103934665603ULL in the last iteration.
    return (iters > 0) ? 1469598103934665603ULL : 0;
  }

  uint64_t final_hash = 0;
  const size_t num_segments = offsets.size() - 1;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t s = 0; s < num_segments; ++s) {
      const uint32_t len = offsets[s + 1] - offsets[s];
      const uint32_t val = static_cast<uint32_t>(s);
      
      // Unroll the mix loop for performance
      uint32_t i = 0;
      while (i + 3 < len) {
        hash = mix(hash, val);
        hash = mix(hash, val);
        hash = mix(hash, val);
        hash = mix(hash, val);
        i += 4;
      }
      while (i < len) {
        hash = mix(hash, val);
        i++;
      }
    }
    final_hash = hash;
  }
  
  return final_hash;
}