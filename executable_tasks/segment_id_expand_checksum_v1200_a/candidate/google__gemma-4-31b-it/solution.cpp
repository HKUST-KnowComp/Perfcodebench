#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters <= 0) return 0;
  
  // The original code resets the hash to 1469598103934665603ULL at the start of every iteration.
  // Therefore, only the final iteration determines the return value.
  uint64_t hash = 1469598103934665603ULL;
  
  const size_t n = offsets.size();
  if (n < 2) return hash;

  for (size_t s = 0; s + 1 < n; ++s) {
    const uint32_t len = offsets[s + 1] - offsets[s];
    const uint32_t val = static_cast<uint32_t>(s);
    
    // Unroll the inner loop to improve throughput
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
  
  return hash;
}