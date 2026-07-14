#include "interface.h"

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
  
  // Build hash set once for O(1) lookups
  std::unordered_set<uint32_t> cache_set;
  cache_set.reserve(sorted_cache_keys.size());
  for (uint32_t key : sorted_cache_keys) {
    cache_set.insert(key);
  }
  
  uint64_t hash = 0;
  const std::size_t num_requests = request_offsets.size() - 1;
  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < num_requests; ++r) {
      uint64_t prefix = 0;
      const int start = offsets_ptr[r];
      const int end = offsets_ptr[r + 1];
      for (int i = start; i < end; ++i) {
        if (cache_set.find(blocks_ptr[i]) == cache_set.end()) {
          break;
        }
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}