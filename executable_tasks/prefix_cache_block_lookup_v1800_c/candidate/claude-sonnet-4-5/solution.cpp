#include "interface.h"

#include <algorithm>

#ifdef __SSE2__
#include <emmintrin.h>
#endif

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Galloping search: exponential search followed by binary search
inline const uint32_t* galloping_search(
    const uint32_t* begin,
    const uint32_t* end,
    uint32_t value) {
  const uint32_t* base = begin;
  std::size_t size = end - begin;
  
  if (size == 0 || value < *base) return end;
  
  // Exponential search phase
  std::size_t step = 1;
  while (step < size && base[step] < value) {
    step <<= 1;
  }
  
  // Binary search in the bounded range
  std::size_t left = step >> 1;
  std::size_t right = (step < size) ? step : size - 1;
  
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
  
  return (left < size && base[left] == value) ? (base + left) : end;
}

// SIMD-accelerated linear search for small arrays
inline bool simd_contains(const uint32_t* data, std::size_t size, uint32_t value) {
#ifdef __SSE2__
  if (size >= 4) {
    __m128i target = _mm_set1_epi32(value);
    std::size_t i = 0;
    for (; i + 4 <= size; i += 4) {
      __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
      __m128i cmp = _mm_cmpeq_epi32(chunk, target);
      if (_mm_movemask_epi8(cmp) != 0) {
        return true;
      }
    }
    // Check remaining elements
    for (; i < size; ++i) {
      if (data[i] == value) return true;
    }
    return false;
  }
#endif
  // Fallback for small sizes or no SSE2
  for (std::size_t i = 0; i < size; ++i) {
    if (data[i] == value) return true;
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
  const std::size_t cache_size = sorted_cache_keys.size();
  const uint32_t* req_data = request_blocks.data();
  
  // Use linear/SIMD search for small caches
  const bool use_linear = cache_size < 64;
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (std::size_t r = 0; r + 1 < request_offsets.size(); ++r) {
      const int start = request_offsets[r];
      const int end = request_offsets[r + 1];
      uint64_t prefix = 0;
      
      const uint32_t* search_start = cache_begin;
      
      for (int i = start; i < end; ++i) {
        const uint32_t block = req_data[i];
        
        // Prefetch next block
        if (i + 1 < end) {
          __builtin_prefetch(&req_data[i + 1], 0, 3);
        }
        
        bool found;
        if (use_linear) {
          found = simd_contains(cache_begin, cache_size, block);
        } else {
          // Use galloping search with hint from previous position
          const uint32_t* result = galloping_search(search_start, cache_end, block);
          found = (result != cache_end && *result == block);
          if (found) {
            search_start = result;
          }
        }
        
        if (!found) break;
        ++prefix;
      }
      
      hash = mix(hash, prefix);
    }
  }
  return hash;
}