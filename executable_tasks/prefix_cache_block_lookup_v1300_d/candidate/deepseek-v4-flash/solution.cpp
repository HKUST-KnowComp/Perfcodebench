#include "interface.h"

#include <algorithm>
#include <unordered_set>
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
  (void)hash_space;
  
  // Build an unordered_set for O(1) membership queries.
  const std::unordered_set<uint32_t> cache_set(sorted_cache_keys.begin(), sorted_cache_keys.end());
  
  const std::size_t num_requests = (request_offsets.size() > 1) ? request_offsets.size() - 1 : 0;
  std::vector<uint64_t> prefix_lengths(num_requests);
  
  // Precompute the longest cached prefix for each request.
  for (std::size_t r = 0; r < num_requests; ++r) {
    const int start = request_offsets[r];
    const int end = request_offsets[r + 1];
    uint64_t prefix = 0;
    for (int i = start; i < end; ++i) {
      if (cache_set.find(request_blocks[i]) == cache_set.end()) {
        break;
      }
      ++prefix;
    }
    prefix_lengths[r] = prefix;
  }
  
  // Compute the checksum by iterating iters times over the precomputed lengths.
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < num_requests; ++r) {
      hash = mix(hash, prefix_lengths[r]);
    }
  }
  
  return hash;
}
