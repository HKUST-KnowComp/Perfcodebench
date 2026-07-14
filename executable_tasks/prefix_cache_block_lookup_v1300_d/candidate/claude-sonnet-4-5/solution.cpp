#include "interface.h"

#include <algorithm>
#include <immintrin.h>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Galloping search: exponential search followed by binary search
inline const uint32_t* galloping_search(const uint32_t* begin, const uint32_t* end, uint32_t value) {
  const uint32_t* base = begin;
  std::size_t size = end - begin;
  
  if (size == 0) return end;
  
  // Exponential search phase
  std::size_t bound = 1;
  while (bound < size && base[bound] < value) {
    bound <<= 1;
  }
  
  // Binary search in the identified range
  std::size_t left = bound >> 1;
  std::size_t right = (bound < size) ? bound : size - 1;
  
  while (left <= right) {
    std::size_t mid = left + ((right - left) >> 1);
    if (base[mid] < value) {
      left = mid + 1;
    } else if (base[mid] > value) {
      if (mid == 0) break;
      right = mid - 1;
    } else {
      return base + mid;
    }
  }
  
  return (left < size && base[left] == value) ? base + left : end;
}

// SIMD-accelerated linear search for small ranges
inline bool simd_contains(const uint32_t* begin, const uint32_t* end, uint32_t value) {
  std::size_t size = end - begin;
  
  // For very small sizes, use simple linear search
  if (size <= 4) {
    for (std::size_t i = 0; i < size; ++i) {
      if (begin[i] == value) return true;
      if (begin[i] > value) return false;
    }
    return false;
  }
  
  // SIMD search for larger ranges
  __m256i target = _mm256_set1_epi32(value);
  std::size_t i = 0;
  
  for (; i + 8 <= size; i += 8) {
    __m256i data = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(begin + i));
    __m256i cmp = _mm256_cmpeq_epi32(data, target);
    int mask = _mm256_movemask_epi8(cmp);
    if (mask) return true;
    
    // Early exit if we've passed the value
    if (begin[i + 7] > value) return false;
  }
  
  // Handle remaining elements
  for (; i < size; ++i) {
    if (begin[i] == value) return true;
    if (begin[i] > value) return false;
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
  
  const uint32_t* cache_begin = sorted_cache_keys.data();
  const uint32_t* cache_end = cache_begin + sorted_cache_keys.size();
  const uint32_t* blocks_data = request_blocks.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      int start = request_offsets[r];
      int end = request_offsets[r + 1];
      uint64_t prefix = 0;
      
      const uint32_t* search_start = cache_begin;
      
      for (int i = start; i < end; ++i) {
        uint32_t block = blocks_data[i];
        
        // Prefetch next block
        if (i + 1 < end) {
          __builtin_prefetch(&blocks_data[i + 1], 0, 3);
        }
        
        // Use galloping search with maintained position
        std::size_t remaining = cache_end - search_start;
        
        if (remaining < 32) {
          // For small remaining ranges, use SIMD linear search
          if (!simd_contains(search_start, cache_end, block)) {
            break;
          }
        } else {
          // Use galloping search for larger ranges
          const uint32_t* found = galloping_search(search_start, cache_end, block);
          if (found == cache_end) {
            break;
          }
          // Update search start for next iteration (exploit sequential pattern)
          search_start = found;
        }
        
        ++prefix;
      }
      
      hash = mix(hash, prefix);
    }
  }
  
  return hash;
}
