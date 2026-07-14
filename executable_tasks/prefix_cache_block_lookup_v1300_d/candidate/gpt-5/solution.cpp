#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Simple open-addressing hash set for uint32_t keys with linear probing.
// - No deletions, so we can terminate search on first empty slot.
// - Power-of-two capacity for fast masking.
struct UInt32HashSet {
  std::vector<uint32_t> table;
  std::vector<uint8_t> occ; // 0 = empty, 1 = occupied
  uint32_t mask;            // table.size() - 1

  UInt32HashSet() : mask(0) {}

  explicit UInt32HashSet(std::size_t n) { init(n); }

  void init(std::size_t n) {
    if (n == 0) {
      table.clear();
      occ.clear();
      mask = 0u;
      return;
    }
    std::size_t cap = 1;
    // target load factor <= 0.5
    std::size_t min_cap = (n << 1);
    while (cap < min_cap) cap <<= 1;
    table.assign(cap, 0u);
    occ.assign(cap, 0u);
    mask = static_cast<uint32_t>(cap - 1);
  }

  static inline uint32_t hash32(uint32_t x) {
    // 32-bit mix; multiplicative hashing is sufficient for linear probing
    return x * 2654435761u;
  }

  void insert(uint32_t key) {
    if (occ.empty()) return; // nothing to do when capacity is zero
    uint32_t idx = hash32(key) & mask;
    while (occ[idx]) {
      if (table[idx] == key) return; // already present
      idx = (idx + 1) & mask;
    }
    occ[idx] = 1u;
    table[idx] = key;
  }

  inline bool contains(uint32_t key) const {
    if (occ.empty()) return false;
    uint32_t idx = hash32(key) & mask;
    while (occ[idx]) {
      if (table[idx] == key) return true;
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
    uint32_t /*hash_space*/,
    int /*iters*/) {
  // Build fast membership structure from cache keys
  UInt32HashSet cache_set(sorted_cache_keys.size());
  for (std::size_t i = 0; i < sorted_cache_keys.size(); ++i) {
    cache_set.insert(sorted_cache_keys[i]);
  }

  const std::size_t num_requests = request_offsets.size() > 0 ? (request_offsets.size() - 1) : 0;

  // Compute per-request prefix lengths once (baseline effectively returns the last iteration only)
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t r = 0; r < num_requests; ++r) {
    int start = request_offsets[r];
    int end = request_offsets[r + 1];
    uint64_t prefix = 0;

    // Guard against malformed offsets
    if (start < 0) start = 0;
    if (end < start) end = start;
    const std::size_t ustart = static_cast<std::size_t>(start);
    const std::size_t uend = static_cast<std::size_t>(end);

    for (std::size_t i = ustart; i < uend; ++i) {
      if (!cache_set.contains(request_blocks[i])) {
        break;
      }
      ++prefix;
    }
    hash = mix(hash, prefix);
  }

  return hash;
}
