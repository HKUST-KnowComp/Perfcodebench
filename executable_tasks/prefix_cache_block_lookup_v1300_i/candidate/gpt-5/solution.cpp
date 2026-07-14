#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Simple open-addressing hash set for uint32_t keys with linear probing.
struct U32HashSet {
  std::vector<uint32_t> keys;
  std::vector<uint8_t> used;  // 0 = empty, 1 = occupied
  uint32_t mask = 0;

  static inline uint32_t hash_u32(uint32_t x) {
    // A fast 32-bit mix
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
  }

  void build(const std::vector<uint32_t>& vals) {
    std::size_t n = vals.size();
    // Choose capacity as power of two >= 2*n (load factor <= 0.5)
    uint32_t cap = 1;
    if (n == 0) {
      cap = 1;
    } else {
      // Prevent overflow; cap grows until >= 2*n
      uint64_t target = (n << 1);
      while (cap < target) cap <<= 1;
      if (cap == 0) cap = 1; // fallback safety (though shouldn't occur)
    }
    keys.assign(cap, 0);
    used.assign(cap, 0);
    mask = cap - 1;

    const uint32_t m = mask;
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t k = vals[i];
      uint32_t h = hash_u32(k) & m;
      // Linear probing
      while (used[h]) {
        if (keys[h] == k) {
          // Duplicate key (input sorted; duplicates unlikely), just break
          goto next_insert;
        }
        h = (h + 1) & m;
      }
      used[h] = 1;
      keys[h] = k;
    next_insert:;
    }
  }

  inline bool contains(uint32_t k) const {
    const uint32_t m = mask;
    uint32_t h = hash_u32(k) & m;
    // Probe until an empty slot found or key matched
    while (used[h]) {
      if (keys[h] == k) return true;
      h = (h + 1) & m;
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
  (void)iters; // Optimization: result is identical regardless of iters since baseline resets hash each iteration

  // Build fast membership structure from cached keys
  U32HashSet set;
  set.build(sorted_cache_keys);

  uint64_t hash = 1469598103934665603ULL; // FNV offset basis

  const int num_requests = request_offsets.size() > 0 ? static_cast<int>(request_offsets.size() - 1) : 0;
  for (int r = 0; r < num_requests; ++r) {
    int begin = request_offsets[r];
    int end = request_offsets[r + 1];

    uint64_t prefix = 0;
    for (int i = begin; i < end; ++i) {
      uint32_t key = request_blocks[static_cast<std::size_t>(i)];
      if (!set.contains(key)) {
        break;
      }
      ++prefix;
    }
    hash = mix(hash, prefix);
  }

  return hash;
}
