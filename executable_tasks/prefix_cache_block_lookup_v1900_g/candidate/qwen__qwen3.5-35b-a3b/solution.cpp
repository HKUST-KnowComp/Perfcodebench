#include "interface.h"

#include <algorithm>
#include <vector>

namespace {

__attribute__((always_inline))
uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  (void)hash_space;
  uint64_t hash = 0;
  
  // Reserve search position to avoid reallocations
  std::vector<std::size_t> search_positions;
  search_positions.reserve(request_offsets.size());
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      uint64_t prefix = 0;
      std::size_t search_pos = 0;
      
      for (int i = request_offsets[r]; i < request_offsets[r + 1]; ++i) {
        uint32_t block = request_blocks[static_cast<std::size_t>(i)];
        
        // Use lower_bound with hint for faster search
        auto it = std::lower_bound(
          sorted_cache_keys.begin() + search_pos,
          sorted_cache_keys.end(),
          block);
        
        if (it != sorted_cache_keys.end() && *it == block) {
          ++prefix;
          // Advance search position for next block
          search_pos = static_cast<std::size_t>(std::distance(sorted_cache_keys.begin(), it)) + 1;
        } else {
          break;
        }
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}