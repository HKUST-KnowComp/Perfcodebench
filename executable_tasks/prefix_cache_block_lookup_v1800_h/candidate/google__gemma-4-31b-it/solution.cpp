#include "interface.h"

#include <vector>
#include <algorithm>

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
  
  // Use a simple hash table for O(1) lookups instead of O(log N) binary search.
  // Since we need to handle potential collisions and the hash_space is given,
  // we can use a flat array if hash_space is reasonable, or a hash set.
  // However, the most performant way for this specific pattern is often a 
  // boolean array if the keys are in a small range, or a hash table.
  
  // Given the constraints, we'll use a hash table with linear probing.
  // To avoid repeated allocation, we allocate once.
  size_t cache_size = sorted_cache_keys.size();
  if (cache_size == 0) {
    uint64_t total_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      uint64_t h = 1469598103934665603ULL;
      for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
        h = mix(h, 0);
      }
      total_hash = mix(total_hash, h);
    }
    // The original code resets hash = 1469... inside the loop and returns the last one.
    // Let's follow the original logic exactly.
  }

  // Optimization: Use a hash table for O(1) lookup
  // We use a power-of-two size for the hash table to use bitmasking instead of modulo.
  size_t table_size = 1;
  while (table_size < cache_size * 2) table_size <<= 1;
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
    uint64_t h = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      uint64_t prefix = 0;
      int start = request_offsets[r];
      int end = request_offsets[r + 1];
      for (int i = start; i < end; ++i) {
        uint32_t key = request_blocks[static_cast<std::size_t>(i)];
        uint32_t pos = key & mask;
        bool found = false;
        while (occupied[pos]) {
          if (table[pos] == key) {
            found = true;
            break;
          }
          pos = (pos + 1) & mask;
        }
        if (!found) break;
        ++prefix;
      }
      h = mix(h, prefix);
    }
    final_hash = h;
  }
  return final_hash;
}