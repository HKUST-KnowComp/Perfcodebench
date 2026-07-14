#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>
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
  
  const std::size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
  if (num_requests == 0) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }

  // Use bitset for O(1) lookup if hash_space is reasonable
  // 64M entries = 8MB bitset, reasonable for L3 cache
  constexpr uint32_t MAX_BITSET_SIZE = 64 * 1024 * 1024;
  
  std::vector<uint64_t> bitset;
  const bool use_bitset = (hash_space > 0 && hash_space <= MAX_BITSET_SIZE);
  
  if (use_bitset) {
    const std::size_t num_words = (hash_space + 63) / 64;
    bitset.resize(num_words, 0);
    for (uint32_t key : sorted_cache_keys) {
      if (key < hash_space) {
        bitset[key >> 6] |= (1ULL << (key & 63));
      }
    }
  }

  const uint32_t* cache_begin = sorted_cache_keys.data();
  const uint32_t* cache_end = cache_begin + sorted_cache_keys.size();
  const uint32_t* blocks_data = request_blocks.data();
  const int* offsets_data = request_offsets.data();
  const uint64_t* bitset_data = use_bitset ? bitset.data() : nullptr;

  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    if (use_bitset) {
      // Fast path with bitset lookup
      for (std::size_t r = 0; r < num_requests; ++r) {
        const int start = offsets_data[r];
        const int end = offsets_data[r + 1];
        uint64_t prefix = 0;
        
        for (int i = start; i < end; ++i) {
          const uint32_t key = blocks_data[static_cast<std::size_t>(i)];
          const uint64_t word = bitset_data[key >> 6];
          const uint64_t mask = 1ULL << (key & 63);
          if (__builtin_expect((word & mask) == 0, 0)) {
            break;
          }
          ++prefix;
        }
        hash = mix(hash, prefix);
      }
    } else {
      // Fallback to binary search
      for (std::size_t r = 0; r < num_requests; ++r) {
        const int start = offsets_data[r];
        const int end = offsets_data[r + 1];
        uint64_t prefix = 0;
        
        for (int i = start; i < end; ++i) {
          const uint32_t key = blocks_data[static_cast<std::size_t>(i)];
          if (!std::binary_search(cache_begin, cache_end, key)) {
            break;
          }
          ++prefix;
        }
        hash = mix(hash, prefix);
      }
    }
  }
  
  return hash;
}