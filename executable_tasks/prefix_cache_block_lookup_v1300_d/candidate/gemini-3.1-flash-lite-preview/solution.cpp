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
  const uint32_t* cache_ptr = sorted_cache_keys.data();
  const size_t cache_size = sorted_cache_keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      int start = request_offsets[r];
      int end = request_offsets[r + 1];
      int prefix = 0;
      
      for (int i = start; i < end; ++i) {
        uint32_t val = request_blocks[i];
        // Use binary search to check existence
        if (std::binary_search(cache_ptr, cache_ptr + cache_size, val)) {
          prefix++;
        } else {
          break;
        }
      }
      hash = mix(hash, static_cast<uint64_t>(prefix));
    }
  }
  return hash;
}