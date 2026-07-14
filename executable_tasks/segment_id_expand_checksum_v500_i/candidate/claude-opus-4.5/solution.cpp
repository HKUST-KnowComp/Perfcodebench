#include "interface.h"

#include <vector>

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  const std::size_t num_segments = offsets.size() > 0 ? offsets.size() - 1 : 0;
  const uint32_t* off_ptr = offsets.data();
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t s = 0; s < num_segments; ++s) {
      const uint32_t len = off_ptr[s + 1] - off_ptr[s];
      const uint64_t seg_id = static_cast<uint64_t>(s);
      for (uint32_t i = 0; i < len; ++i) {
        hash ^= seg_id;
        hash *= 1099511628211ULL;
      }
    }
  }
  return hash;
}