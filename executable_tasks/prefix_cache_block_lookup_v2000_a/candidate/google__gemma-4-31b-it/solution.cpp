#include "interface.h"

#include <vector>
#include <algorithm>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Fast hash table for O(1) lookup
struct FastCache {
    uint32_t* table;
    uint32_t size;
    uint32_t mask;
    static constexpr uint32_t EMPTY = 0xFFFFFFFF;

    FastCache(const std::vector<uint32_t>& keys) {
        size = 1;
        while (size < keys.size() * 2) size <<= 1;
        mask = size - 1;
        table = new uint32_t[size];
        for (uint32_t i = 0; i < size; ++i) table[i] = EMPTY;

        for (uint32_t k : keys) {
            uint32_t h = hash_fn(k) & mask;
            while (table[h] != EMPTY) {
                h = (h + 1) & mask;
            }
            table[h] = k;
        }
    }

    ~FastCache() {
        delete[] table;
    }

    inline bool contains(uint32_t k) const {
        uint32_t h = hash_fn(k) & mask;
        while (table[h] != EMPTY) {
            if (table[h] == k) return true;
            h = (h + 1) & mask;
        }
        return false;
    }

    static inline uint32_t hash_fn(uint32_t x) {
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = (x >> 16) ^ x;
        return x;
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
  
  FastCache cache(sorted_cache_keys);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      uint64_t prefix = 0;
      int start = request_offsets[r];
      int end = request_offsets[r + 1];
      for (int i = start; i < end; ++i) {
        if (!cache.contains(request_blocks[static_cast<std::size_t>(i)])) {
          break;
        }
        ++prefix;
      }
      current_hash = mix(current_hash, prefix);
    }
    final_hash = current_hash;
  }
  
  // The original code resets hash = 1469... inside the loop and returns the last iteration's result
  // To match the baseline exactly, we return the result of the last iteration.
  return final_hash;
}