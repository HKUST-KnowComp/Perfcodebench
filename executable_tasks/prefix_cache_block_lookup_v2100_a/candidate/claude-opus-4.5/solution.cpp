#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstring>

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
  
  // Build a bitset for O(1) cache membership lookup
  const size_t num_words = (static_cast<size_t>(hash_space) + 63) / 64;
  std::vector<uint64_t> bitset(num_words, 0);
  
  for (uint32_t key : sorted_cache_keys) {
    bitset[key >> 6] |= (1ULL << (key & 63));
  }
  
  const uint64_t* bitset_ptr = bitset.data();
  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();
  const size_t num_requests = request_offsets.size() - 1;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (size_t r = 0; r < num_requests; ++r) {
      int start = offsets_ptr[r];
      int end = offsets_ptr[r + 1];
      uint64_t prefix = 0;
      
      for (int i = start; i < end; ++i) {
        uint32_t block = blocks_ptr[i];
        if (!(bitset_ptr[block >> 6] & (1ULL << (block & 63)))) {
          break;
        }
        ++prefix;
      }
      
      hash = mix(hash, prefix);
    }
  }
  
  return hash;
}