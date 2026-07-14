#include "interface.h"
#include <algorithm>
#include <vector>

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
  uint64_t hash = 0;
  
  // Pre-calculate a boolean lookup if the key space is small, 
  // but given the constraints, std::binary_search is often fast if the cache is large.
  // However, we can optimize the search by using std::lower_bound to reduce comparisons.
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      int start = request_offsets[r];
      int end = request_offsets[r + 1];
      int prefix = 0;
      
      for (int i = start; i < end; ++i) {
        // Use lower_bound to find the element; if it exists, it must be at the returned iterator.
        auto it = std::lower_bound(sorted_cache_keys.begin(), sorted_cache_keys.end(), request_blocks[i]);
        if (it != sorted_cache_keys.end() && *it == request_blocks[i]) {
          prefix++;
        } else {
          break;
        }
      }
      hash = mix(hash, prefix);
    }
  }
  return hash;
}