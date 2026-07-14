#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct FlatHashSet {
    std::vector<uint32_t> keys;
    std::vector<uint8_t> used;
    size_t mask;

    FlatHashSet(const std::vector<uint32_t>& sorted_keys) {
        size_t n = sorted_keys.size();
        size_t cap = 1;
        while (cap < n * 2) cap <<= 1;
        keys.resize(cap);
        used.assign(cap, 0);
        mask = cap - 1;
        for (uint32_t k : sorted_keys) {
            size_t h = k;
            h ^= h >> 16;
            h *= 0x85ebca6b;
            h ^= h >> 13;
            h *= 0xc2b2ae35;
            h ^= h >> 16;
            size_t idx = h & mask;
            while (used[idx]) {
                idx = (idx + 1) & mask;
            }
            keys[idx] = k;
            used[idx] = 1;
        }
    }

    bool contains(uint32_t k) const {
        size_t h = k;
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;
        size_t idx = h & mask;
        while (used[idx]) {
            if (keys[idx] == k) return true;
            idx = (idx + 1) & mask;
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
  FlatHashSet cache(sorted_cache_keys);
  uint64_t hash = 0;
  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();
  size_t num_requests = request_offsets.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t r = 0; r + 1 < num_requests; ++r) {
      uint64_t prefix = 0;
      int start = offsets_ptr[r];
      int end = offsets_ptr[r + 1];
      for (int i = start; i < end; ++i) {
        if (!cache.contains(blocks_ptr[i])) {
          break;
        }
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}
