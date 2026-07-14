#include "interface.h"

#include <vector>

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
  // Build hash table from sorted_cache_keys
  std::vector<uint32_t> hash_table(hash_space, UINT32_MAX);
  
  for (uint32_t key : sorted_cache_keys) {
    uint32_t pos = key % hash_space;
    while (hash_table[pos] != UINT32_MAX && hash_table[pos] != key) {
      pos = (pos + 1) % hash_space;
    }
    hash_table[pos] = key;
  }
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      uint64_t prefix = 0;
      for (int i = request_offsets[r]; i < request_offsets[r + 1]; ++i) {
        uint32_t block = request_blocks[static_cast<std::size_t>(i)];
        uint32_t pos = block % hash_space;
        
        // Linear probing lookup
        bool found = false;
        uint32_t start_pos = pos;
        while (hash_table[pos] != UINT32_MAX) {
          if (hash_table[pos] == block) {
            found = true;
            break;
          }
          pos = (pos + 1) % hash_space;
          if (pos == start_pos) break;
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