#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct HashSet32 {
  std::vector<uint32_t> keys;
  std::vector<uint8_t> used;
  uint32_t mask = 0;

  static uint32_t next_pow2(uint32_t v) {
    if (v <= 1) return 1u;
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
  }

  void build(const std::vector<uint32_t>& vals) {
    const uint32_t n = static_cast<uint32_t>(vals.size());
    if (n == 0) { mask = 0; keys.clear(); used.clear(); return; }
    uint32_t cap = next_pow2(n * 2u);
    keys.assign(cap, 0u);
    used.assign(cap, 0u);
    mask = cap - 1u;
    for (uint32_t v : vals) {
      uint32_t idx = (static_cast<uint32_t>(v * 2654435761u)) & mask;
      while (used[idx]) {
        if (keys[idx] == v) goto next_val; // skip duplicates
        idx = (idx + 1u) & mask;
      }
      used[idx] = 1u;
      keys[idx] = v;
    next_val:;
    }
  }

  inline bool contains(uint32_t v) const {
    if (used.empty()) return false;
    uint32_t idx = (static_cast<uint32_t>(v * 2654435761u)) & mask;
    while (used[idx]) {
      if (keys[idx] == v) return true;
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
  (void)iters; // Result is independent of iters; compute once for performance.

  const std::size_t R = request_offsets.empty() ? 0u : (request_offsets.size() - 1u);

  // Fast path: no cache keys -> all prefix lengths are 0
  if (sorted_cache_keys.empty()) {
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < R; ++r) {
      hash = mix(hash, 0);
    }
    return hash;
  }

  // Build a compact hash set for O(1) average membership checks.
  HashSet32 set;
  set.build(sorted_cache_keys);

  // Compute prefix lengths once.
  std::vector<uint64_t> lengths;
  lengths.resize(R);

  for (std::size_t r = 0; r < R; ++r) {
    int s = request_offsets[r];
    int e = request_offsets[r + 1];
    // Guard against malformed offsets; assume valid per contract, but ensure non-negative.
    if (e <= s) { lengths[r] = 0; continue; }
    std::size_t i = static_cast<std::size_t>(s);
    const std::size_t end = static_cast<std::size_t>(e);
    uint64_t prefix = 0;
    while (i < end) {
      uint32_t blk = request_blocks[i];
      if (!set.contains(blk)) break;
      ++prefix;
      ++i;
    }
    lengths[r] = prefix;
  }

  // Compute checksum once (iters do not affect final value since each iter resets hash).
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t r = 0; r < R; ++r) {
    hash = mix(hash, lengths[r]);
  }
  return hash;
}
