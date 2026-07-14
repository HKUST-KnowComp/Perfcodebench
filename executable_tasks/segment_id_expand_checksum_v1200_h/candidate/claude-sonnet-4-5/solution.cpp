#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
      
      for (uint32_t i = 0; i < len; ++i) {
        hash = mix(hash, seg_id);
      }
    }
  }
  
  return hash;
}