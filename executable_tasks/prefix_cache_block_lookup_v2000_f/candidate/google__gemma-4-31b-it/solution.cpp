#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Simple fast hash table for uint32_t keys
struct FastSet {
    uint32_t* table;
    uint32_t size;
    uint32_t mask;
    static constexpr uint32_t EMPTY = 0xFFFFFFFF;

    FastSet(const std::vector<uint32_t>& keys) {
        // Size to power of 2 for fast masking, load factor ~0.5
        size = 1;
        while (size < keys.size() * 2) size <<= 1;
        mask = size - 1;
        table = new uint32_t[size];
        for (uint32_t i = 0; i < size; ++i) table[i] = EMPTY;

        for (uint32_t k : keys) {
            uint32_t h = hash_u32(k) & mask;
            while (table[h] != EMPTY) {
                h = (h + 1) & mask;
            }
            table[h] = k;
        }
    }

    ~FastSet() {
        delete[] table;
    }

    inline bool contains(uint32_t k) const {
        uint32_t h = hash_u32(k) & mask;
        while (table[h] != EMPTY) {
            if (table[h] == k) return true;
            h = (h + 1) & mask;
        }
        return false;
    }

    static inline uint32_t hash_u32(uint32_t x) {
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
  
  // Build a fast lookup set for the cache keys
  FastSet cache_set(sorted_cache_keys);
  
  uint64_t final_hash = 0;
  const size_t num_requests = request_offsets.size() - 1;
  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = 1469598103934665603ULL;
    for (size_t r = 0; r < num_requests; ++r) {
      int start = offsets_ptr[r];
      int end = offsets_ptr[r + 1];
      uint64_t prefix = 0;
      for (int i = start; i < end; ++i) {
        if (!cache_set.contains(blocks_ptr[i])) {
          break;
        }
        ++prefix;
      }
      current_hash = mix(current_hash, prefix);
    }
    // The original code resets hash = 1469... inside the loop and returns the last iter's result
    // but the logic implies we are calculating the checksum for the last iteration or accumulating.
    // Looking at the baseline: hash is reset every iter, so only the last iter's result is returned.
    final_hash = current_hash;
  }
  
  return final_hash;
}
