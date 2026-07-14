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
  
  // Build a bitset for O(1) lookup
  // hash_space tells us the range of possible keys
  const size_t bitset_size = (static_cast<size_t>(hash_space) + 63) / 64;
  std::vector<uint64_t> cache_bitset(bitset_size, 0);
  
  for (uint32_t key : sorted_cache_keys) {
    if (key < hash_space) {
      cache_bitset[key >> 6] |= (1ULL << (key & 63));
    }
  }
  
  const uint64_t* bitset_ptr = cache_bitset.data();
  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();
  const size_t num_requests = request_offsets.size() - 1;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (size_t r = 0; r < num_requests; ++r) {
      const int start = offsets_ptr[r];
      const int end = offsets_ptr[r + 1];
      uint64_t prefix = 0;
      
      for (int i = start; i < end; ++i) {
        uint32_t block = blocks_ptr[i];
        // O(1) bitset lookup
        uint64_t word = bitset_ptr[block >> 6];
        uint64_t mask = 1ULL << (block & 63);
        if (!(word & mask)) {
          break;
        }
        ++prefix;
      }
      
      hash = mix(hash, prefix);
    }
  }
  
  return hash;
}