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
  
  const uint32_t* keys_ptr = sorted_cache_keys.data();
  const size_t keys_size = sorted_cache_keys.size();
  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();
  const size_t num_requests = request_offsets.size() - 1;

  uint64_t final_hash = 0;

  // The logic is independent of 'iters' except for the checksum accumulation.
  // However, the problem asks for the checksum of the per-request lengths.
  // The baseline resets the hash inside the iters loop, meaning it calculates
  // the same checksum 'iters' times. We can compute it once and then
  // handle the 'iters' loop if the checksum is meant to be cumulative or
  // if the baseline's behavior of resetting the hash is the intended logic.
  // Looking at the baseline: it resets 'hash' to a constant every iteration.
  // This means the result of the function is simply the result of one iteration.

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_hash = 1469598103934665603ULL;
    for (size_t r = 0; r < num_requests; ++r) {
      int start = offsets_ptr[r];
      int end = offsets_ptr[r + 1];
      uint64_t prefix = 0;
      
      for (int i = start; i < end; ++i) {
        uint32_t val = blocks_ptr[i];
        // Fast binary search
        if (!std::binary_search(sorted_cache_keys.begin(), sorted_cache_keys.end(), val)) {
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
