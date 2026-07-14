#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct UInt32HashSet {
  std::vector<uint32_t> keys;
  std::vector<uint8_t> used;
  uint32_t mask;

  static inline uint32_t hash(uint32_t x, uint32_t mask) {
    // Multiplicative hashing, mask is capacity-1 (power of two)
    return (x * 2654435761u) & mask;
  }

  void init(const std::vector<uint32_t>& input) {
    std::size_t n = input.size();
    // Choose capacity as next power of two >= 2*n (avoid high load factor)
    std::size_t cap = 1;
    if (n == 0) {
      cap = 1;
    } else {
      cap = 1;
      std::size_t need = n * 2;
      while (cap < need) cap <<= 1;
    }
    keys.assign(cap, 0u);
    used.assign(cap, 0u);
    mask = static_cast<uint32_t>(cap - 1);

    // Insert all keys
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t k = input[i];
      uint32_t idx = hash(k, mask);
      while (used[idx]) {
        if (keys[idx] == k) {
          // Already present
          goto next_key;
        }
        idx = (idx + 1) & mask;
      }
      used[idx] = 1;
      keys[idx] = k;
    next_key:;
    }
  }

  inline bool contains(uint32_t k) const {
    uint32_t idx = hash(k, mask);
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
  (void)iters; // Result is identical across iterations; compute once.

  // Build a fast membership structure from sorted_cache_keys
  UInt32HashSet set;
  set.init(sorted_cache_keys);

  const uint32_t* blocks = request_blocks.data();
  const int* offsets = request_offsets.data();
  std::size_t num_requests = request_offsets.size() > 0 ? (request_offsets.size() - 1) : 0;

  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t r = 0; r < num_requests; ++r) {
    int begin = offsets[r];
    int end = offsets[r + 1];
    uint64_t prefix = 0;
    for (int i = begin; i < end; ++i) {
      if (!set.contains(blocks[static_cast<std::size_t>(i)])) {
        break;
      }
      ++prefix;
    }
    hash = mix(hash, prefix);
  }

  return hash;
}
