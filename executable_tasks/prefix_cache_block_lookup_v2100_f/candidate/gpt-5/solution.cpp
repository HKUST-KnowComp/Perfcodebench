#include "interface.h"

#include <cstddef>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// 32-bit mixing hash for better distribution
static inline uint32_t hash32(uint32_t x) {
  x ^= x >> 16;
  x *= 0x7feb352dU;
  x ^= x >> 15;
  x *= 0x846ca68bU;
  x ^= x >> 16;
  return x;
}

struct FlatHashSet {
  std::vector<uint32_t> keys;
  std::vector<uint8_t> used; // 0 = empty, 1 = occupied
  uint32_t mask = 0;

  void init(std::size_t n) {
    // choose capacity as power of two >= max(1, n*2) for load factor <= 0.5
    uint32_t need = n > 0 ? static_cast<uint32_t>(n * 2u) : 1u;
    uint32_t cap = 1u;
    while (cap < need) cap <<= 1;
    keys.assign(cap, 0u);
    used.assign(cap, 0u);
    mask = cap - 1u;
  }

  void insert(uint32_t k) {
    uint32_t idx = hash32(k) & mask;
    while (used[idx]) {
      if (keys[idx] == k) {
        return; // duplicate
      }
      idx = (idx + 1u) & mask;
    }
    used[idx] = 1u;
    keys[idx] = k;
  }

  bool contains(uint32_t k) const {
    uint32_t idx = hash32(k) & mask;
    while (used[idx]) {
      if (keys[idx] == k) return true;
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

  // Build a fast membership structure once
  FlatHashSet set;
  set.init(sorted_cache_keys.size());
  for (std::size_t i = 0; i < sorted_cache_keys.size(); ++i) {
    set.insert(sorted_cache_keys[i]);
  }

  // Precompute prefix lengths per request once (they don't change across iters)
  const std::size_t num_requests = request_offsets.size() > 0 ? (request_offsets.size() - 1) : 0;
  std::vector<uint64_t> prefixes;
  prefixes.resize(num_requests);

  const std::size_t total_blocks = request_blocks.size();
  for (std::size_t r = 0; r < num_requests; ++r) {
    int start = request_offsets[r];
    int end = request_offsets[r + 1];
    if (start < 0) start = 0;
    if (end < start) end = start;
    if (static_cast<std::size_t>(start) > total_blocks) {
      prefixes[r] = 0;
      continue;
    }
    if (static_cast<std::size_t>(end) > total_blocks) {
      end = static_cast<int>(total_blocks);
    }
    uint64_t prefix = 0;
    for (int i = start; i < end; ++i) {
      uint32_t key = request_blocks[static_cast<std::size_t>(i)];
      if (!set.contains(key)) {
        break;
      }
      ++prefix;
    }
    prefixes[r] = prefix;
  }

  // Perform mixing per iteration using the precomputed prefixes
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL; // FNV offset basis
    for (std::size_t r = 0; r < num_requests; ++r) {
      hash = mix(hash, prefixes[r]);
    }
  }
  return hash;
}