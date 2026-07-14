#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

class HashTable {
  std::vector<uint32_t> table;
  std::vector<uint8_t> used;
  size_t mask;

public:
  explicit HashTable(size_t capacity) {
    size_t cap = 1;
    while (cap < capacity * 2) cap <<= 1;
    table.resize(cap);
    used.resize(cap, 0);
    mask = cap - 1;
  }

  void insert(uint32_t key) {
    size_t i = key & mask;
    while (used[i]) {
      if (table[i] == key) return;
      i = (i + 1) & mask;
    }
    table[i] = key;
    used[i] = 1;
  }

  bool contains(uint32_t key) const {
    size_t i = key & mask;
    while (used[i]) {
      if (table[i] == key) return true;
      i = (i + 1) & mask;
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
  (void)hash_space;

  // Pre‑build a hash table for O(1) membership tests.
  HashTable ht(sorted_cache_keys.size());
  for (uint32_t key : sorted_cache_keys) {
    ht.insert(key);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const std::size_t num_blocks = request_offsets.size() - 1;
    for (std::size_t r = 0; r < num_blocks; ++r) {
      uint64_t prefix = 0;
      int start = request_offsets[r];
      int end = request_offsets[r + 1];
      for (int i = start; i < end; ++i) {
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
