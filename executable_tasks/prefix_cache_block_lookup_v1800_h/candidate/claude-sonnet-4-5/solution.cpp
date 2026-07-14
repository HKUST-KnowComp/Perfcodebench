#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

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
  
  // Build hash table once
  std::vector<uint32_t> hash_table(hash_space, UINT32_MAX);
  uint32_t mask = hash_space - 1;
  
  for (uint32_t key : sorted_cache_keys) {
    uint32_t pos = key & mask;
    while (hash_table[pos] != UINT32_MAX) {
      pos = (pos + 1) & mask;
    }
    hash_table[pos] = key;
  }
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      uint64_t prefix = 0;
      int start = request_offsets[r];
      int end = request_offsets[r + 1];
      
      for (int i = start; i < end; ++i) {
        uint32_t block = request_blocks[static_cast<std::size_t>(i)];
        uint32_t pos = block & mask;
        
        // Prefetch next block if available
        if (i + 1 < end) {
          uint32_t next_block = request_blocks[static_cast<std::size_t>(i + 1)];
          uint32_t next_pos = next_block & mask;
          __builtin_prefetch(&hash_table[next_pos], 0, 1);
        }
        
        // Linear probing lookup
        bool found = false;
        while (hash_table[pos] != UINT32_MAX) {
          if (hash_table[pos] == block) {
            found = true;
            break;
          }
          pos = (pos + 1) & mask;
        }
        
        if (!found) {
          break;
        }
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}