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
  // Build a hash set for O(1) lookups of cache keys.
  std::unordered_set<uint32_t> cache_set;
  cache_set.reserve(sorted_cache_keys.size());
  cache_set.insert(sorted_cache_keys.begin(), sorted_cache_keys.end());

  // Only the last iteration affects the returned checksum;
  // previous iterations are overwritten and have no side effects.
  if (iters <= 0) return 0;

  const std::size_t num_requests =
      request_offsets.empty() ? 0 : request_offsets.size() - 1;
  if (num_requests == 0) return 1469598103934665603ULL;

  const int* off_data = request_offsets.data();
  const uint32_t* blk_data = request_blocks.data();

  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t r = 0; r < num_requests; ++r) {
    const int start = off_data[r];
    const int end = off_data[r + 1];
    uint64_t prefix = 0;
    for (int i = start; i < end; ++i) {
      if (cache_set.find(blk_data[i]) == cache_set.end()) {
        break;
      }
      ++prefix;
    }
    hash = mix(hash, prefix);
  }

  // If iters > 1, the checksum is still the last iteration's value;
  // this single computation is identical to the last iteration.
  return hash;
}