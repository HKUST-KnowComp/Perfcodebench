#include "interface.h"
#include <vector>
#include <cstdint>

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
} // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  (void)hash_space;
  
  uint64_t final_hash = 0;
  const size_t num_requests = request_offsets.size() - 1;
  const size_t num_keys = sorted_cache_keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    
    for (size_t r = 0; r < num_requests; ++r) {
      int start = request_offsets[r];
      int end = request_offsets[r + 1];
      int prefix = 0;
      
      size_t key_idx = 0;
      for (int i = start; i < end; ++i) {
        uint32_t block = request_blocks[i];
        
        // Advance key_idx to find the block
        while (key_idx < num_keys && sorted_cache_keys[key_idx] < block) {
          key_idx++;
        }
        
        if (key_idx < num_keys && sorted_cache_keys[key_idx] == block) {
          prefix++;
        } else {
          break;
        }
      }
      hash = mix(hash, prefix);
    }
    final_hash = hash;
  }
  return final_hash;
}