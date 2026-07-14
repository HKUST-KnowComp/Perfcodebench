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
  
  // Build a bitset for O(1) lookup if hash_space is reasonable
  // For very large hash_space, fall back to binary search with optimizations
  std::vector<uint64_t> bitset;
  bool use_bitset = false;
  
  if (hash_space > 0 && hash_space <= 256 * 1024 * 1024) {
    // Use bitset for O(1) lookup
    size_t num_words = (static_cast<size_t>(hash_space) + 63) / 64;
    bitset.resize(num_words, 0);
    for (uint32_t key : sorted_cache_keys) {
      if (key < hash_space) {
        bitset[key >> 6] |= (1ULL << (key & 63));
      }
    }
    use_bitset = true;
  }
  
  const size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();
  const uint64_t* bitset_ptr = use_bitset ? bitset.data() : nullptr;
  const uint32_t* cache_begin = sorted_cache_keys.data();
  const uint32_t* cache_end = cache_begin + sorted_cache_keys.size();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    if (use_bitset) {
      // Fast path with bitset lookup
      for (size_t r = 0; r < num_requests; ++r) {
        int start = offsets_ptr[r];
        int end = offsets_ptr[r + 1];
        uint64_t prefix = 0;
        
        for (int i = start; i < end; ++i) {
          uint32_t block = blocks_ptr[i];
          uint64_t word = bitset_ptr[block >> 6];
          uint64_t mask = 1ULL << (block & 63);
          if (!(word & mask)) {
            break;
          }
          ++prefix;
        }
        hash = mix(hash, prefix);
      }
    } else {
      // Fallback: optimized binary search
      for (size_t r = 0; r < num_requests; ++r) {
        int start = offsets_ptr[r];
        int end = offsets_ptr[r + 1];
        uint64_t prefix = 0;
        
        for (int i = start; i < end; ++i) {
          uint32_t block = blocks_ptr[i];
          // Manual binary search to avoid function call overhead
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
          if (!found) {
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