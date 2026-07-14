#include "interface.h"

#include <vector>

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
  // Precompute single cache presence bitmask for O(1) lookups across all iterations
  std::vector<bool> is_cached(hash_space, false);
  for (uint32_t key : sorted_cache_keys) {
    is_cached[key] = true;
  }

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      uint64_t prefix = 0;
      const int start = request_offsets[r];
      const int end = request_offsets[r + 1];
      for (int i = start; i < end; ++i) {
        const uint32_t block = request_blocks[static_cast<size_t>(i)];
        if (!is_cached[block]) {
          break;
        }
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
    final_hash = hash;
  }
  return final_hash;
}