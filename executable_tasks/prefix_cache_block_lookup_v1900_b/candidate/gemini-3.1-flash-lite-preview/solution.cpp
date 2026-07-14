#include "interface.h"
#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  
  // Use a vector as a direct-address table for O(1) lookup.
  // hash_space is the range of possible block values.
  std::vector<bool> is_cached(hash_space, false);
  for (uint32_t key : sorted_cache_keys) {
    if (key < hash_space) {
      is_cached[key] = true;
    }
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      int prefix = 0;
      int start = request_offsets[r];
      int end = request_offsets[r + 1];
      for (int i = start; i < end; ++i) {
        uint32_t block = request_blocks[static_cast<std::size_t>(i)];
        if (block >= hash_space || !is_cached[block]) {
          break;
        }
        ++prefix;
      }
      hash = mix(hash, static_cast<uint64_t>(prefix));
    }
  }
  return hash;
}