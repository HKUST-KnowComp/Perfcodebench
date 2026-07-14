// Optimized solution: use hash set for O(1) block lookups
#include "interface.h"
#include <algorithm>
#include <unordered_set>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  (void)hash_space;
  // Build a fast lookup set once (outside iteration loop)
  std::unordered_set<uint32_t> cache_set;
  cache_set.reserve(sorted_cache_keys.size());
  cache_set.insert(sorted_cache_keys.begin(), sorted_cache_keys.end());

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      int start = request_offsets[r];
      int end = request_offsets[r + 1];
      uint64_t prefix = 0;
      for (int i = start; i < end; ++i) {
        if (cache_set.find(request_blocks[static_cast<std::size_t>(i)]) == cache_set.end()) {
          break;
        }
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}