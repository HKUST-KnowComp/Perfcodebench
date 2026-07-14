#include "interface.h"

#include <algorithm>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Custom binary search to avoid iterator overhead and potentially improve inlining
inline bool fast_binary_search(const uint32_t* begin, const uint32_t* end, uint32_t val) {
  uint32_t* low = begin;
  uint32_t* high = end - 1;
  while (low <= high) {
    uint32_t* mid = low + (high - low) / 2;
    if (*mid == val) return true;
    if (*mid < val) low = mid + 1;
    else high = mid - 1;
  }
  return false;
}

}  // namespace

uint64_t prefix_cache_checksum(
    const std::vector<uint32_t>& sorted_cache_keys,
    const std::vector<uint32_t>& request_blocks,
    const std::vector<int>& request_offsets,
    uint32_t hash_space,
    int iters) {
  (void)hash_space;
  
  const uint32_t* cache_ptr = sorted_cache_keys.data();
  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();
  const uint32_t* cache_end = cache_ptr + sorted_cache_keys.size();
  
  uint64_t final_hash = 0;
  
  // The problem asks for the checksum of the per-request lengths.
  // The baseline resets the hash inside the iters loop, but the return is the final hash.
  // Actually, the baseline returns the hash of the LAST iteration.
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = 1469598103934665603ULL;
    size_t num_requests = offsets_ptr[0] == 0 ? (request_offsets.size() - 1) : (request_offsets.size() - 1);
    // The baseline loop: for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r)
    for (size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      int start = offsets_ptr[r];
      int end = offsets_ptr[r + 1];
      uint64_t prefix = 0;
      for (int i = start; i < end; ++i) {
        if (!fast_binary_search(cache_ptr, cache_end, blocks_ptr[i])) {
          break;
        }
        ++prefix;
      }
      current_hash = mix(current_hash, prefix);
    }
    final_hash = current_hash;
  }
  
  return final_hash;
}
