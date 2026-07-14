#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Fast hash set for uint32_t using linear probing
struct FastSet {
    uint32_t* table;
    uint32_t size;
    uint32_t mask;
    bool* occupied;

    FastSet(const std::vector<uint32_t>& keys) {
        size = 1;
        while (size < keys.size() * 2) size <<= 1;
        mask = size - 1;
        table = new uint32_t[size];
        occupied = new bool[size]();

        for (uint32_t k : keys) {
            uint32_t h = k & mask;
            while (occupied[h]) {
                h = (h + 1) & mask;
            }
            table[h] = k;
            occupied[h] = true;
        }
    }

    ~FastSet() {
        delete[] table;
        delete[] occupied;
    }

    inline bool contains(uint32_t k) const {
        uint32_t h = k & mask;
        while (occupied[h]) {
            if (table[h] == k) return true;
            h = (h + 1) & mask;
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
  
  // Pre-build a hash set for O(1) lookups instead of O(log N) binary search
  FastSet cache_set(sorted_cache_keys);
  
  uint64_t total_hash = 0;
  const size_t num_requests = request_offsets.size() - 1;
  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_hash = 1469598103934665603ULL;
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
      iter_hash = mix(iter_hash, prefix);
    }
    // The original code resets hash = 1469... inside the loop and returns the last iter's hash
    // but the logic implies it wants the result of the final iteration.
    total_hash = iter_hash;
  }
  
  return total_hash;
}