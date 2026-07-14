#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>
#include <cstring>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#define HAS_AVX2 1
#else
#define HAS_AVX2 0
#endif

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Bitset for O(1) lookup
class Bitset {
public:
  std::vector<uint64_t> data;
  
  void init(uint32_t max_val) {
    size_t num_words = (static_cast<size_t>(max_val) + 64) / 64;
    data.assign(num_words, 0);
  }
  
  void set(uint32_t val) {
    data[val >> 6] |= (1ULL << (val & 63));
  }
  
  bool test(uint32_t val) const {
    return (data[val >> 6] >> (val & 63)) & 1;
  }
};

} // namespace

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
  
  // Build bitset for O(1) lookup if hash_space is reasonable
  // 128MB limit for bitset
  const size_t max_bitset_bytes = 128ULL * 1024 * 1024;
  const bool use_bitset = (static_cast<size_t>(hash_space) / 8 + 8) <= max_bitset_bytes;
  
  Bitset cache_bitset;
  if (use_bitset) {
    cache_bitset.init(hash_space);
    for (uint32_t key : sorted_cache_keys) {
      cache_bitset.set(key);
    }
  }
  
  // Precompute prefix lengths for all requests
  std::vector<uint64_t> prefix_lengths(num_requests);
  
  const uint32_t* blocks_ptr = request_blocks.data();
  const int* offsets_ptr = request_offsets.data();
  
  if (use_bitset) {
    const uint64_t* bitset_data = cache_bitset.data.data();
    
    for (size_t r = 0; r < num_requests; ++r) {
      int start = offsets_ptr[r];
      int end = offsets_ptr[r + 1];
      uint64_t prefix = 0;
      
#if HAS_AVX2
      // Process 8 blocks at a time with SIMD
      int i = start;
      for (; i + 8 <= end; i += 8) {
        // Load 8 block indices
        __m256i blocks = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(blocks_ptr + i));
        
        // Compute word indices (block >> 6) and bit positions (block & 63)
        __m256i word_idx = _mm256_srli_epi32(blocks, 6);
        __m256i bit_pos = _mm256_and_si256(blocks, _mm256_set1_epi32(63));
        
        // Gather the words from bitset
        __m256i words = _mm256_i32gather_epi64(reinterpret_cast<const long long*>(bitset_data), 
                                                _mm_loadu_si128(reinterpret_cast<const __m128i*>(&word_idx)), 8);
        __m256i words2 = _mm256_i32gather_epi64(reinterpret_cast<const long long*>(bitset_data),
                                                 _mm_loadu_si128(reinterpret_cast<const __m128i*>(reinterpret_cast<const int*>(&word_idx) + 4)), 8);
        
        // Extract bit positions for each half
        int bp0 = _mm256_extract_epi32(bit_pos, 0);
        int bp1 = _mm256_extract_epi32(bit_pos, 1);
        int bp2 = _mm256_extract_epi32(bit_pos, 2);
        int bp3 = _mm256_extract_epi32(bit_pos, 3);
        int bp4 = _mm256_extract_epi32(bit_pos, 4);
        int bp5 = _mm256_extract_epi32(bit_pos, 5);
        int bp6 = _mm256_extract_epi32(bit_pos, 6);
        int bp7 = _mm256_extract_epi32(bit_pos, 7);
        
        // Check each bit
        uint64_t w0 = static_cast<uint64_t>(_mm256_extract_epi64(words, 0));
        uint64_t w1 = static_cast<uint64_t>(_mm256_extract_epi64(words, 1));
        uint64_t w2 = static_cast<uint64_t>(_mm256_extract_epi64(words, 2));
        uint64_t w3 = static_cast<uint64_t>(_mm256_extract_epi64(words, 3));
        uint64_t w4 = static_cast<uint64_t>(_mm256_extract_epi64(words2, 0));
        uint64_t w5 = static_cast<uint64_t>(_mm256_extract_epi64(words2, 1));
        uint64_t w6 = static_cast<uint64_t>(_mm256_extract_epi64(words2, 2));
        uint64_t w7 = static_cast<uint64_t>(_mm256_extract_epi64(words2, 3));
        
        int hit0 = (w0 >> bp0) & 1;
        int hit1 = (w1 >> bp1) & 1;
        int hit2 = (w2 >> bp2) & 1;
        int hit3 = (w3 >> bp3) & 1;
        int hit4 = (w4 >> bp4) & 1;
        int hit5 = (w5 >> bp5) & 1;
        int hit6 = (w6 >> bp6) & 1;
        int hit7 = (w7 >> bp7) & 1;
        
        // Find first miss
        if (!hit0) goto done;
        prefix++;
        if (!hit1) goto done;
        prefix++;
        if (!hit2) goto done;
        prefix++;
        if (!hit3) goto done;
        prefix++;
        if (!hit4) goto done;
        prefix++;
        if (!hit5) goto done;
        prefix++;
        if (!hit6) goto done;
        prefix++;
        if (!hit7) goto done;
        prefix++;
      }
      
      // Handle remaining blocks
      for (; i < end; ++i) {
        uint32_t block = blocks_ptr[i];
        if (!((bitset_data[block >> 6] >> (block & 63)) & 1)) {
          break;
        }
        ++prefix;
      }
      done:
#else
      for (int i = start; i < end; ++i) {
        uint32_t block = blocks_ptr[i];
        if (!((bitset_data[block >> 6] >> (block & 63)) & 1)) {
          break;
        }
        ++prefix;
      }
#endif
      prefix_lengths[r] = prefix;
    }
  } else {
    // Fallback to binary search for very large hash_space
    const uint32_t* cache_begin = sorted_cache_keys.data();
    const uint32_t* cache_end = cache_begin + sorted_cache_keys.size();
    
    for (size_t r = 0; r < num_requests; ++r) {
      int start = offsets_ptr[r];
      int end = offsets_ptr[r + 1];
      uint64_t prefix = 0;
      
      for (int i = start; i < end; ++i) {
        if (!std::binary_search(cache_begin, cache_end, blocks_ptr[i])) {
          break;
        }
        ++prefix;
      }
      prefix_lengths[r] = prefix;
    }
  }
  
  // Compute hash for all iterations
  uint64_t hash = 0;
  const uint64_t* pl_ptr = prefix_lengths.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    // Unroll hash computation
    size_t r = 0;
    for (; r + 4 <= num_requests; r += 4) {
      hash = mix(hash, pl_ptr[r]);
      hash = mix(hash, pl_ptr[r + 1]);
      hash = mix(hash, pl_ptr[r + 2]);
      hash = mix(hash, pl_ptr[r + 3]);
    }
    for (; r < num_requests; ++r) {
      hash = mix(hash, pl_ptr[r]);
    }
  }
  
  return hash;
}