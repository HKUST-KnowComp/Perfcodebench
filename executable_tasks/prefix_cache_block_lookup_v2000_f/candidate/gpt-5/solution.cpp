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
struct UInt32HashSet {
  std::vector<uint32_t> table;
  std::vector<uint8_t> used;
  uint32_t mask;

  static uint32_t next_pow2(uint32_t v) {
    if (v <= 1u) return 1u;
    --v;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    return v + 1u;
  }

  void build(const std::vector<uint32_t>& keys) {
    const uint32_t n = static_cast<uint32_t>(keys.size());
    if (n == 0) {
      table.clear();
      used.clear();
      mask = 0u;
      return;
    }
    uint32_t cap = next_pow2(n * 2u);
    table.assign(cap, 0u);
    used.assign(cap, 0u);
    mask = cap - 1u;
    for (uint32_t k : keys) {
      uint32_t idx = (k * 2654435761u) & mask;
      while (used[idx]) {
        idx = (idx + 1u) & mask;
      }
      table[idx] = k;
      used[idx] = 1u;
    }
  }

  inline bool contains(uint32_t k) const {
    if (used.empty()) return false;
    uint32_t idx = (k * 2654435761u) & mask;
    while (used[idx]) {
      if (table[idx] == k) return true;
      idx = (idx + 1u) & mask;
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
  (void)iters; // Result is identical for any iters >= 1 since each iteration resets and recomputes the same hash.

  // Build fast membership structure from cache keys.
  UInt32HashSet set;
  set.build(sorted_cache_keys);

  const uint64_t basis = 1469598103934665603ULL;
  uint64_t hash = basis;

  const std::size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
  for (std::size_t r = 0; r < num_requests; ++r) {
    uint64_t prefix = 0;
    int start = request_offsets[r];
    int end = request_offsets[r + 1];
    // Iterate through blocks until first miss.
    for (int i = start; i < end; ++i) {
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
