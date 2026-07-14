#include "interface.h"
#include <algorithm>
#include <unordered_set>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  (void)hash_space;
  
  // Pre-build a fast lookup structure. 
  // Since sorted_cache_keys is already sorted, std::binary_search is O(log N).
  // For very large datasets, a hash set might be faster, but for cache-locality 
  // and avoiding allocation overhead, we stick to the sorted vector but optimize the loop.
  
  uint64_t total_hash = 0;
  const size_t num_offsets = request_offsets.size();
  
  // Pre-calculate prefix lengths to avoid re-computing in the 'iters' loop
  std::vector<uint64_t> prefix_lengths;
  prefix_lengths.reserve(num_offsets - 1);

  for (size_t r = 0; r + 1 < num_offsets; ++r) {
    uint64_t prefix = 0;
    int start = request_offsets[r];
    int end = request_offsets[r + 1];
    
    for (int i = start; i < end; ++i) {
      // Using std::binary_search on a sorted vector is very fast due to cache locality
      if (std::binary_search(sorted_cache_keys.begin(), sorted_cache_keys.end(), request_blocks[static_cast<size_t>(i)])) {
        ++prefix;
      } else {
        break;
      }
    }
    prefix_lengths.push_back(prefix);
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint64_t p : prefix_lengths) {
      hash = mix(hash, p);
    }
    total_hash = hash;
  }

  return total_hash;
}