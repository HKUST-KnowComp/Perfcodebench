#include "interface.h"
#include <algorithm>
#include <vector>

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
  uint64_t total_hash = 0;
  
  // Pre-calculate the prefix lengths for all blocks to avoid redundant binary searches
  // However, the request_blocks are accessed via request_offsets. 
  // The bottleneck is the binary search inside the loop.
  
  // Optimization: Use a more efficient way to check existence.
  // Since sorted_cache_keys is sorted, std::binary_search is O(log M).
  // We can optimize the inner loop by noting that we only care about the first block that fails.

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      uint64_t prefix = 0;
      const int start = request_offsets[r];
      const int end = request_offsets[r + 1];
      
      for (int i = start; i < end; ++i) {
        // Using std::binary_search is already quite fast, but we ensure 
        // the compiler can inline and optimize the loop.
        if (std::binary_search(sorted_cache_keys.begin(), sorted_cache_keys.end(), request_blocks[static_cast<std::size_t>(i)])) {
          ++prefix;
        } else {
          break;
        }
      }
      hash = mix(hash, prefix);
    }
    total_hash = hash;
  }
  
  return total_hash;
}