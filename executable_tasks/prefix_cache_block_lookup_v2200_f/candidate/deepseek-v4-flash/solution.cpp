#include "interface.h"

#include <vector>
#include <cstdint>
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

  // Build fast presence check for cache keys
  std::unordered_set<uint32_t> cache_set;
  cache_set.reserve(sorted_cache_keys.size());
  for (auto key : sorted_cache_keys) {
    cache_set.insert(key);
  }

  // Precompute prefix lengths for each request
  std::size_t num_requests = (request_offsets.size() > 1) ? request_offsets.size() - 1 : 0;
  std::vector<uint64_t> prefix_lengths(num_requests);

  for (std::size_t r = 0; r < num_requests; ++r) {
    uint64_t prefix = 0;
    int start = request_offsets[r];
    int end = request_offsets[r + 1];
    for (int i = start; i < end; ++i) {
      if (cache_set.find(request_blocks[static_cast<std::size_t>(i)]) == cache_set.end()) {
        break;
      }
      ++prefix;
    }
    prefix_lengths[r] = prefix;
  }

  // Compute checksum by repeating iters times (reusing precomputed lengths)
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r < num_requests; ++r) {
      hash = mix(hash, prefix_lengths[r]);
    }
  }

  return hash;
}
