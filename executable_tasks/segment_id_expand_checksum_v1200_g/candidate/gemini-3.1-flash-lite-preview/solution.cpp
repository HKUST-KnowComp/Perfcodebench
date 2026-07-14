#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

// FNV-1a style mix function
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (offsets.size() < 2) return 0;

  uint64_t final_hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    
    for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
      const uint32_t len = offsets[s + 1] - offsets[s];
      const uint64_t val = static_cast<uint64_t>(s);
      
      // Process the range [s] repeated 'len' times
      // Unrolling slightly for performance
      uint32_t i = 0;
      for (; i + 3 < len; i += 4) {
        hash = mix(hash, val);
        hash = mix(hash, val);
        hash = mix(hash, val);
        hash = mix(hash, val);
      }
      for (; i < len; ++i) {
        hash = mix(hash, val);
      }
    }
    final_hash = hash;
  }
  
  return final_hash;
}