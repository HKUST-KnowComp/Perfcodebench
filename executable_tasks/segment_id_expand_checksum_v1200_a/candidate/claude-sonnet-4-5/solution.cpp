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
  uint64_t hash = 0;
  const std::size_t num_segments = offsets.size() - 1;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (std::size_t s = 0; s < num_segments; ++s) {
      const uint32_t len = offsets[s + 1] - offsets[s];
      const uint32_t seg_id = static_cast<uint32_t>(s);
      
      // Unroll by 4 for better ILP
      uint32_t i = 0;
      const uint32_t len_unroll = len & ~3u;
      
      uint64_t h0 = hash;
      uint64_t h1 = hash;
      uint64_t h2 = hash;
      uint64_t h3 = hash;
      
      for (; i < len_unroll; i += 4) {
        h0 = mix(h0, seg_id);
        h1 = mix(h1, seg_id);
        h2 = mix(h2, seg_id);
        h3 = mix(h3, seg_id);
      }
      
      // Combine the parallel hashes
      hash = h0;
      hash = mix(hash, h1);
      hash = mix(hash, h2);
      hash = mix(hash, h3);
      
      // Handle remainder
      for (; i < len; ++i) {
        hash = mix(hash, seg_id);
      }
    }
  }
  return hash;
}