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

  // Build a bitset for O(1) membership lookup
  const size_t bitset_size = (static_cast<size_t>(hash_space) + 63) / 64;
  std::vector<uint64_t> bitset(bitset_size, 0);
  
  for (uint32_t key : sorted_cache_keys) {
    if (key < hash_space) {
      bitset[key >> 6] |= (1ULL << (key & 63));
    }
  }

  const size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
  
  // Precompute prefix lengths for all requests
  std::vector<uint64_t> prefix_lengths(num_requests);
  
  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();
  const uint64_t* bitset_ptr = bitset.data();
  
  for (size_t r = 0; r < num_requests; ++r) {
    int start = offsets_ptr[r];
    int end = offsets_ptr[r + 1];
    uint64_t prefix = 0;
    
    int i = start;
    // Process blocks one by one, checking bitset
    for (; i < end; ++i) {
      uint32_t block = blocks_ptr[i];
      if (block >= hash_space) {
        break;
      }
      uint64_t word = bitset_ptr[block >> 6];
      uint64_t mask = 1ULL << (block & 63);
      if (!(word & mask)) {
        break;
      }
      ++prefix;
    }
    
    prefix_lengths[r] = prefix;
  }

  // Compute hash over iterations
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t r = 0; r < num_requests; ++r) {
      hash = mix(hash, prefix_lengths[r]);
    }
  }
  
  return hash;
}