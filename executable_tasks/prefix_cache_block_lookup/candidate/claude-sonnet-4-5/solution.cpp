#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct HashTable {
  std::vector<uint32_t> table;
  uint32_t mask;
  
  HashTable(const std::vector<uint32_t>& sorted_cache_keys, uint32_t hash_space) {
    uint32_t size = hash_space;
    // Ensure power of 2
    if ((size & (size - 1)) != 0) {
      size = 1;
      while (size < hash_space) size <<= 1;
    }
    mask = size - 1;
    table.resize(size, UINT32_MAX);
    
    for (uint32_t key : sorted_cache_keys) {
      uint32_t pos = key & mask;
      while (table[pos] != UINT32_MAX && table[pos] != key) {
        pos = (pos + 1) & mask;
      }
      table[pos] = key;
    }
  }
  
  inline bool contains(uint32_t key) const {
    uint32_t pos = key & mask;
    while (table[pos] != UINT32_MAX) {
      if (table[pos] == key) return true;
      pos = (pos + 1) & mask;
    }
    return false;
  }
};

}  // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  HashTable ht(sorted_cache_keys, hash_space);
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      uint64_t prefix = 0;
      for (int i = request_offsets[r]; i < request_offsets[r + 1]; ++i) {
        if (!ht.contains(request_blocks[static_cast<std::size_t>(i)])) {
          break;
        }
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}