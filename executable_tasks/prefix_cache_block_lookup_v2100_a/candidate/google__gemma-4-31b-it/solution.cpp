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
  
  // Pre-calculate the cache size to avoid repeated calls to .size()
  const size_t cache_size = sorted_cache_keys.size();
  const uint32_t* cache_ptr = sorted_cache_keys.data();
  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();
  const size_t num_requests = request_offsets.size() - 1;

  uint64_t final_hash = 0;

  // The outer loop is just for repeated runs as per the task description
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = 1469598103934665603ULL;
    
    for (size_t r = 0; r < num_requests; ++r) {
      int start = offsets_ptr[r];
      int end = offsets_ptr[r + 1];
      uint64_t prefix = 0;

      for (int i = start; i < end; ++i) {
        uint32_t val = blocks_ptr[i];
        
        // Optimized binary search: std::binary_search is fine, but we can 
        // ensure the compiler inlines it or use a manual implementation.
        // For most modern compilers, std::binary_search on a contiguous 
        // vector is highly optimized.
        if (!std::binary_search(cache_ptr, cache_ptr + cache_size, val)) {
          break;
        }
        ++prefix;
      }
      current_hash = mix(current_hash, prefix);
    }
    final_hash = current_hash;
  }
  
  // The task asks for the checksum of the per-request lengths. 
  // The baseline returns the hash of the last iteration.
  return final_hash;
}