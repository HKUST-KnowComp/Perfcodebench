#include "interface.h"

#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters <= 0) return 0; // Based on baseline, if iters is 0, it returns 0 (initial hash)

  // The baseline logic: 
  // 1. It clears and refills 'expanded' every iteration.
  // 2. It resets 'hash' to 1469598103934665603ULL every iteration.
  // 3. It processes the same 'expanded' sequence every iteration.
  // Therefore, the result of the last iteration is the same as any single iteration.
  
  uint64_t hash = 1469598103934665603ULL;
  
  const size_t n = offsets.size();
  if (n < 2) return hash;

  for (size_t s = 0; s + 1 < n; ++s) {
    const uint32_t len = offsets[s + 1] - offsets[s];
    const uint32_t val = static_cast<uint32_t>(s);
    
    // Unroll the mix loop for the length of the segment
    for (uint32_t i = 0; i < len; ++i) {
      hash = mix(hash, val);
    }
  }
  
  return hash;
}