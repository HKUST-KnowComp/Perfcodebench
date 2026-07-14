#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>
#include <unordered_set>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

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
  if (num_requests == 0 || iters <= 0) {
    return 1469598103934665603ULL;
  }

  // Convert offsets to size_t once
  std::vector<std::size_t> offsets(request_offsets.size());
  for (std::size_t i = 0; i < request_offsets.size(); ++i) {
    offsets[i] = static_cast<std::size_t>(request_offsets[i]);
  }

  const uint32_t* blocks_ptr = request_blocks.data();
  const std::size_t* offsets_ptr = offsets.data();

  // Use bitset for O(1) lookup if hash_space is reasonable
  constexpr uint32_t MAX_BITSET_SIZE = 64 * 1024 * 1024; // 64M entries = 8MB memory
  
  uint64_t hash = 0;

  if (hash_space <= MAX_BITSET_SIZE && hash_space > 0) {
    // Build bitset
    std::vector<uint64_t> bitset((hash_space + 63) / 64, 0);
    for (uint32_t key : sorted_cache_keys) {
      if (key < hash_space) {
        bitset[key >> 6] |= (1ULL << (key & 63));
      }
    }

    const uint64_t* bitset_ptr = bitset.data();

    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;

#if defined(__AVX2__)
      // SIMD path: process 8 blocks at a time
      for (std::size_t r = 0; r < num_requests; ++r) {
        std::size_t start = offsets_ptr[r];
        std::size_t end = offsets_ptr[r + 1];
        std::size_t len = end - start;
        uint64_t prefix = 0;
        std::size_t i = 0;

        // Process 8 at a time
        while (i + 8 <= len) {
          // Load 8 block keys
          __m256i keys = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(blocks_ptr + start + i));
          
          // Extract and check each key in bitset
          alignas(32) uint32_t key_arr[8];
          _mm256_store_si256(reinterpret_cast<__m256i*>(key_arr), keys);
          
          uint32_t mask = 0;
          for (int j = 0; j < 8; ++j) {
            uint32_t k = key_arr[j];
            if ((bitset_ptr[k >> 6] & (1ULL << (k & 63))) != 0) {
              mask |= (1u << j);
            }
          }
          
          if (mask == 0xFF) {
            prefix += 8;
            i += 8;
          } else {
            // Find first zero bit (first miss)
            int first_miss = __builtin_ctz(~mask);
            prefix += first_miss;
            goto done_request;
          }
        }

        // Handle remaining blocks
        while (i < len) {
          uint32_t k = blocks_ptr[start + i];
          if ((bitset_ptr[k >> 6] & (1ULL << (k & 63))) == 0) {
            break;
          }
          ++prefix;
          ++i;
        }

        done_request:
        hash = mix(hash, prefix);
      }
#else
      // Scalar path with bitset
      for (std::size_t r = 0; r < num_requests; ++r) {
        std::size_t start = offsets_ptr[r];
        std::size_t end = offsets_ptr[r + 1];
        uint64_t prefix = 0;
        
        for (std::size_t i = start; i < end; ++i) {
          uint32_t k = blocks_ptr[i];
          if ((bitset_ptr[k >> 6] & (1ULL << (k & 63))) == 0) {
            break;
          }
          ++prefix;
        }
        hash = mix(hash, prefix);
      }
#endif
    }
  } else {
    // Fallback: use unordered_set for O(1) average lookup
    std::unordered_set<uint32_t> cache_set(sorted_cache_keys.begin(), sorted_cache_keys.end());
    
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
      
      for (std::size_t r = 0; r < num_requests; ++r) {
        std::size_t start = offsets_ptr[r];
        std::size_t end = offsets_ptr[r + 1];
        uint64_t prefix = 0;
        
        for (std::size_t i = start; i < end; ++i) {
          if (cache_set.find(blocks_ptr[i]) == cache_set.end()) {
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