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
  
  // Use a simple open-addressing hash table for O(1) lookups.
  // We use a size that is a power of two for fast masking.
  size_t table_size = 1;
  while (table_size < sorted_cache_keys.size() * 2) {
    table_size <<= 1;
  }
  
  std::vector<uint32_t> table(table_size, 0xFFFFFFFF);
  std::vector<bool> occupied(table_size, false);
  uint32_t mask = static_cast<uint32_t>(table_size - 1);

  for (uint32_t key : sorted_cache_keys) {
    uint32_t h = key & mask;
    while (occupied[h]) {
      h = (h + 1) & mask;
    }
    table[h] = key;
    occupied[h] = true;
  }

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      uint64_t prefix = 0;
      int start = request_offsets[r];
      int end = request_offsets[r + 1];
      
      for (int i = start; i < end; ++i) {
        uint32_t block = request_blocks[static_cast<std::size_t>(i)];
        uint32_t h = block & mask;
        bool found = false;
        while (occupied[h]) {
          if (table[h] == block) {
            found = true;
            break;
          }
          h = (h + 1) & mask;
        }
        
        if (!found) break;
        ++prefix;
      }
      current_iter_hash = mix(current_iter_hash, prefix);
    }
    final_hash = mix(final_hash, current_iter_hash);
  }

  // The original baseline resets hash = 1469598103934665603ULL inside the loop
  // and returns the hash of the LAST iteration. Let's match that exactly.
  
  uint64_t last_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h_val = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      uint64_t prefix = 0;
      for (int i = request_offsets[r]; i < request_offsets[r + 1]; ++i) {
        uint32_t block = request_blocks[static_cast<std::size_t>(i)];
        uint32_t h = block & mask;
        bool found = false;
        while (occupied[h]) {
          if (table[h] == block) { found = true; break; }
          h = (h + 1) & mask;
        }
        if (!found) break;
        ++prefix;
      }
      h_val = mix(h_val, prefix);
    }
    last_hash = h_val;
  }
  
  return last_hash;
}