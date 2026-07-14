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

  // Build a hash set of cached keys for O(1) lookups
  std::unordered_set<uint32_t> cache_set(sorted_cache_keys.begin(), sorted_cache_keys.end());

  // Precompute the longest prefix length for each request
  size_t num_requests = request_offsets.empty() ? 0 : request_offsets.size() - 1;
  std::vector<uint64_t> prefix_lengths;
  prefix_lengths.reserve(num_requests);

  for (size_t r = 0; r < num_requests; ++r) {
    int start = request_offsets[r];
    int end = request_offsets[r + 1];
    uint64_t len = 0;
    for (int i = start; i < end; ++i) {
      if (cache_set.find(request_blocks[static_cast<size_t>(i)]) == cache_set.end()) {
        break;
      }
      ++len;
    }
    prefix_lengths.push_back(len);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t r = 0; r < prefix_lengths.size(); ++r) {
      hash = mix(hash, prefix_lengths[r]);
    }
  }
  return hash;
}