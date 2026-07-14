#include "interface.h"

#include <cstdint>
#include <vector>
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
  
  std::unordered_set<uint32_t> cache_set;
  cache_set.reserve(sorted_cache_keys.size());
  for (uint32_t key : sorted_cache_keys) {
    cache_set.insert(key);
  }
  
  std::vector<uint64_t> prefix_lengths;
  const int num_requests = static_cast<int>(request_offsets.size()) - 1;
  if (num_requests > 0) {
    prefix_lengths.reserve(num_requests);
    for (int r = 0; r < num_requests; ++r) {
      int start = request_offsets[r];
      int end = request_offsets[r + 1];
      uint64_t prefix = 0;
      for (int i = start; i < end; ++i) {
        if (cache_set.find(request_blocks[i]) == cache_set.end()) {
          break;
        }
        ++prefix;
      }
      prefix_lengths.push_back(prefix);
    }
  }
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint64_t prefix : prefix_lengths) {
      hash = mix(hash, prefix);
    }
  }
  return hash;
}
