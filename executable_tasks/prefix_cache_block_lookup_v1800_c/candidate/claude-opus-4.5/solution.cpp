#include "interface.h"

#include <algorithm>
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
  
  const size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
  if (num_requests == 0) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }
  
  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();
  
  // Use bitset for O(1) lookup if hash_space is reasonable
  constexpr uint32_t MAX_BITSET_SIZE = 64 * 1024 * 1024; // 64M entries = 8MB memory
  
  std::vector<uint64_t> bitset;
  bool use_bitset = (hash_space > 0 && hash_space <= MAX_BITSET_SIZE);
  
  if (use_bitset) {
    size_t num_words = (hash_space + 63) / 64;
    bitset.resize(num_words, 0);
    for (uint32_t key : sorted_cache_keys) {
      if (key < hash_space) {
        bitset[key >> 6] |= (1ULL << (key & 63));
      }
    }
  }
  
  const uint32_t* cache_begin = sorted_cache_keys.data();
  const uint32_t* cache_end = cache_begin + sorted_cache_keys.size();
  
  uint64_t hash = 0;
  
  if (use_bitset) {
    const uint64_t* bitset_ptr = bitset.data();
    
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
      
      for (size_t r = 0; r < num_requests; ++r) {
        int start = offsets_ptr[r];
        int end = offsets_ptr[r + 1];
        uint64_t prefix = 0;
        
        for (int i = start; i < end; ++i) {
          uint32_t block = blocks_ptr[i];
          if (__builtin_expect(block < hash_space && (bitset_ptr[block >> 6] & (1ULL << (block & 63))), 1)) {
            ++prefix;
          } else {
            break;
          }
        }
        hash = mix(hash, prefix);
      }
    }
  } else {
    // Fallback to binary search
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
      
      for (size_t r = 0; r < num_requests; ++r) {
        int start = offsets_ptr[r];
        int end = offsets_ptr[r + 1];
        uint64_t prefix = 0;
        
        for (int i = start; i < end; ++i) {
          uint32_t block = blocks_ptr[i];
          // Manual binary search for speed
          const uint32_t* lo = cache_begin;
          const uint32_t* hi = cache_end;
          bool found = false;
          while (lo < hi) {
            const uint32_t* mid = lo + ((hi - lo) >> 1);
            if (*mid < block) {
              lo = mid + 1;
            } else if (*mid > block) {
              hi = mid;
            } else {
              found = true;
              break;
            }
          }
          if (__builtin_expect(found, 1)) {
            ++prefix;
          } else {
            break;
          }
        }
        hash = mix(hash, prefix);
      }
    }
  }
  
  return hash;
}