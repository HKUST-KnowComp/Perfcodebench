#include "interface.h"

#include <unordered_set>

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
  (void)hash_space;

  // Build hash set for O(1) lookups
  std::unordered_set<uint32_t> cache_set(sorted_cache_keys.begin(), sorted_cache_keys.end());

  const std::size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
  const int* offsets = request_offsets.data();
  const uint32_t* blocks = request_blocks.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < num_requests; ++r) {
      const int start = offsets[r];
      const int end = offsets[r + 1];
      uint64_t prefix = 0;
      for (int i = start; i < end; ++i) {
        if (cache_set.find(blocks[i]) == cache_set.end()) {
          break;
        }
        ++prefix;
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}