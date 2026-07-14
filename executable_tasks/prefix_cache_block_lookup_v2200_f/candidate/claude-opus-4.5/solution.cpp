#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Fast bitset lookup
inline bool bitset_contains(const uint64_t* bitset, uint32_t key) {
  return (bitset[key >> 6] >> (key & 63)) & 1;
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
    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 1; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }

  // Use bitset for O(1) lookup if hash_space is reasonable
  constexpr uint32_t MAX_BITSET_SIZE = 64 * 1024 * 1024; // 64M keys = 8MB bitset
  
  std::vector<uint64_t> bitset;
  const uint64_t* bitset_ptr = nullptr;
  
  if (hash_space <= MAX_BITSET_SIZE && hash_space > 0) {
    size_t bitset_words = (hash_space + 63) / 64;
    bitset.resize(bitset_words, 0);
    for (uint32_t key : sorted_cache_keys) {
      if (key < hash_space) {
        bitset[key >> 6] |= (1ULL << (key & 63));
      }
    }
    bitset_ptr = bitset.data();
  }

  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();
  const uint32_t* cache_begin = sorted_cache_keys.data();
  const uint32_t* cache_end = cache_begin + sorted_cache_keys.size();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    if (bitset_ptr) {
      // Fast path: bitset lookup
      for (size_t r = 0; r < num_requests; ++r) {
        int start = offsets_ptr[r];
        int end = offsets_ptr[r + 1];
        uint64_t prefix = 0;
        
#if defined(__AVX2__)
        // Process 8 blocks at a time with SIMD
        int i = start;
        while (i + 8 <= end) {
          // Load 8 keys
          __m256i keys = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(blocks_ptr + i));
          
          // Check each key in bitset
          uint32_t k[8];
          _mm256_storeu_si256(reinterpret_cast<__m256i*>(k), keys);
          
          uint32_t mask = 0;
          for (int j = 0; j < 8; ++j) {
            if (bitset_contains(bitset_ptr, k[j])) {
              mask |= (1u << j);
            }
          }
          
          if (mask == 0xFF) {
            prefix += 8;
            i += 8;
          } else {
            // Find first miss
            int first_miss = __builtin_ctz(~mask);
            prefix += first_miss;
            goto done_request;
          }
        }
        
        // Handle remaining blocks
        while (i < end) {
          if (__builtin_expect(!bitset_contains(bitset_ptr, blocks_ptr[i]), 0)) {
            goto done_request;
          }
          ++prefix;
          ++i;
        }
        done_request:
#else
        for (int i = start; i < end; ++i) {
          if (__builtin_expect(!bitset_contains(bitset_ptr, blocks_ptr[i]), 0)) {
            break;
          }
          ++prefix;
        }
#endif
        hash = mix(hash, prefix);
      }
    } else {
      // Fallback: binary search with prefetching
      for (size_t r = 0; r < num_requests; ++r) {
        int start = offsets_ptr[r];
        int end = offsets_ptr[r + 1];
        uint64_t prefix = 0;
        
        for (int i = start; i < end; ++i) {
          uint32_t key = blocks_ptr[i];
          
          // Prefetch next key's potential cache line
          if (__builtin_expect(i + 1 < end, 1)) {
            uint32_t next_key = blocks_ptr[i + 1];
            size_t mid_idx = sorted_cache_keys.size() / 2;
            __builtin_prefetch(&cache_begin[mid_idx], 0, 0);
          }
          
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