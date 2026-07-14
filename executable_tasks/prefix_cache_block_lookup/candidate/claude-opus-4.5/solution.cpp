#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

#if defined(__AVX2__)
#include <immintrin.h>
#define USE_AVX2 1
#else
#define USE_AVX2 0
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
  
  const size_t num_requests = request_offsets.size() > 0 ? request_offsets.size() - 1 : 0;
  if (num_requests == 0) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }

  // Determine max key to size the bitset appropriately
  uint32_t max_key = 0;
  if (!sorted_cache_keys.empty()) {
    max_key = sorted_cache_keys.back();
  }
  
  // Also check request blocks for max value
  uint32_t max_block = 0;
  if (!request_blocks.empty()) {
    for (size_t i = 0; i < request_blocks.size(); ++i) {
      if (request_blocks[i] > max_block) max_block = request_blocks[i];
    }
  }
  
  const uint32_t bitset_size = std::max(max_key, max_block) + 1;
  const size_t num_words = (bitset_size + 63) / 64;
  
  // Build bitset for O(1) lookup
  std::vector<uint64_t> cache_bitset(num_words, 0);
  for (uint32_t key : sorted_cache_keys) {
    cache_bitset[key >> 6] |= (1ULL << (key & 63));
  }
  
  const uint64_t* bitset_ptr = cache_bitset.data();
  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (size_t r = 0; r < num_requests; ++r) {
      const int start = offsets_ptr[r];
      const int end = offsets_ptr[r + 1];
      int len = end - start;
      
      if (len <= 0) {
        hash = mix(hash, 0);
        continue;
      }
      
      const uint32_t* req_blocks = blocks_ptr + start;
      uint64_t prefix = 0;
      
#if USE_AVX2
      // Process 8 blocks at a time with SIMD
      int i = 0;
      while (i + 8 <= len) {
        // Load 8 block IDs
        __m256i block_ids = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(req_blocks + i));
        
        // Check each block in the bitset
        // Extract individual values and check
        alignas(32) uint32_t ids[8];
        _mm256_store_si256(reinterpret_cast<__m256i*>(ids), block_ids);
        
        bool all_hit = true;
        for (int j = 0; j < 8; ++j) {
          uint32_t id = ids[j];
          if (!(bitset_ptr[id >> 6] & (1ULL << (id & 63)))) {
            prefix += j;
            all_hit = false;
            break;
          }
        }
        
        if (!all_hit) break;
        prefix += 8;
        i += 8;
      }
      
      // Handle remaining blocks
      while (i < len) {
        uint32_t id = req_blocks[i];
        if (!(bitset_ptr[id >> 6] & (1ULL << (id & 63)))) {
          break;
        }
        ++prefix;
        ++i;
      }
#else
      // Scalar path with unrolling
      int i = 0;
      while (i + 4 <= len) {
        uint32_t id0 = req_blocks[i];
        uint32_t id1 = req_blocks[i + 1];
        uint32_t id2 = req_blocks[i + 2];
        uint32_t id3 = req_blocks[i + 3];
        
        bool hit0 = (bitset_ptr[id0 >> 6] & (1ULL << (id0 & 63))) != 0;
        if (!hit0) break;
        
        bool hit1 = (bitset_ptr[id1 >> 6] & (1ULL << (id1 & 63))) != 0;
        if (!hit1) { prefix += 1; break; }
        
        bool hit2 = (bitset_ptr[id2 >> 6] & (1ULL << (id2 & 63))) != 0;
        if (!hit2) { prefix += 2; break; }
        
        bool hit3 = (bitset_ptr[id3 >> 6] & (1ULL << (id3 & 63))) != 0;
        if (!hit3) { prefix += 3; break; }
        
        prefix += 4;
        i += 4;
      }
      
      while (i < len) {
        uint32_t id = req_blocks[i];
        if (!(bitset_ptr[id >> 6] & (1ULL << (id & 63)))) {
          break;
        }
        ++prefix;
        ++i;
      }
#endif
      
      hash = mix(hash, prefix);
    }
  }
  
  return hash;
}