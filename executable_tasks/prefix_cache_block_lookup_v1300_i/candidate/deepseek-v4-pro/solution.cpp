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
  (void)hash_space; // unused in this implementation
  uint64_t hash = 0;

  // Build constant-time lookup structure from the cache keys once.
  std::unordered_set<uint32_t> cache_set(sorted_cache_keys.begin(), sorted_cache_keys.end());
  cache_set.reserve(sorted_cache_keys.size());

  const uint32_t* blocks_data = request_blocks.data();
  const int* offs_data = request_offsets.data();
  const std::size_t num_requests = request_offsets.size() - 1;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < num_requests; ++r) {
      uint64_t prefix = 0;
      const int start = offs_data[r];
      const int end   = offs_data[r + 1];
      for (int i = start; i < end; ++i) {
        if (cache_set.find(blocks_data[i]) == cache_set.end()) {
          break;
        }
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }

  return hash;
}