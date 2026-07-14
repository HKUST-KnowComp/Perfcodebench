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
  
  // Pre-calculate the size of the cache to avoid repeated calls to .size()
  const size_t cache_size = sorted_cache_keys.size();
  const uint32_t* cache_ptr = sorted_cache_keys.data();
  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();
  const size_t num_requests = request_offsets.size() - 1;

  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = 1469598103934665603ULL;
    for (size_t r = 0; r < num_requests; ++r) {
      int start = offsets_ptr[r];
      int end = offsets_ptr[r + 1];
      uint64_t prefix = 0;
      
      for (int i = start; i < end; ++i) {
        uint32_t key = blocks_ptr[i];
        
        // Optimized binary search
        bool found = false;
        int low = 0, high = (int)cache_size - 1;
        while (low <= high) {
          int mid = low + ((high - low) >> 1);
          uint32_t mid_val = cache_ptr[mid];
          if (mid_val == key) {
            found = true;
            break;
          }
          if (mid_val < key) low = mid + 1;
          else high = mid - 1;
        }
        
        if (!found) break;
        ++prefix;
      }
      current_hash = mix(current_hash, prefix);
    }
    final_hash = current_hash;
  }
  
  return final_hash;
}
