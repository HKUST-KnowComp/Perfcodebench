#include "interface.h"
#include <immintrin.h>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  uint64_t sum = 0;
  const uint32_t* vals = values.data();
  const uint32_t* min_ptr = mins.data();
  const uint32_t* max_ptr = maxs.data();
  const size_t n = values.size();
  const size_t num_blocks = (n + block_size - 1) / block_size;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    for (size_t block_idx = 0; block_idx < num_blocks; ++block_idx) {
      const uint32_t block_min = min_ptr[block_idx];
      const uint32_t block_max = max_ptr[block_idx];
      
      // Skip blocks completely outside range
      if (block_max < low || block_min > high) {
        continue;
      }
      
      const size_t block_start = block_idx * block_size;
      const size_t block_end = (block_start + block_size < n) ? block_start + block_size : n;
      const uint32_t* block_vals = vals + block_start;
      const size_t block_len = block_end - block_start;
      
      // If entire block is within range, sum without checks
      if (block_min >= low && block_max <= high) {
        size_t i = 0;
        
        // SIMD processing for aligned blocks
        const size_t simd_end = block_len & ~7;
        __m256i sum_vec = _mm256_setzero_si256();
        
        for (; i < simd_end; i += 8) {
          __m256i v = _mm256_loadu_si256((__m256i*)(block_vals + i));
          sum_vec = _mm256_add_epi64(sum_vec, _mm256_cvtepu32_epi64(_mm256_castsi256_si128(v)));
          sum_vec = _mm256_add_epi64(sum_vec, _mm256_cvtepu32_epi64(_mm256_extracti128_si256(v, 1)));
        }
        
        // Horizontal sum
        __m128i sum_low = _mm256_castsi256_si128(sum_vec);
        __m128i sum_high = _mm256_extracti128_si256(sum_vec, 1);
        sum_low = _mm_add_epi64(sum_low, sum_high);
        sum += _mm_extract_epi64(sum_low, 0) + _mm_extract_epi64(sum_low, 1);
        
        // Scalar tail
        for (; i < block_len; ++i) {
          sum += block_vals[i];
        }
      } else {
        // Partial overlap: check each value
        size_t i = 0;
        
        // SIMD with masking
        const size_t simd_end = block_len & ~7;
        __m256i low_vec = _mm256_set1_epi32(low);
        __m256i high_vec = _mm256_set1_epi32(high);
        __m256i sum_vec = _mm256_setzero_si256();
        
        for (; i < simd_end; i += 8) {
          __m256i v = _mm256_loadu_si256((__m256i*)(block_vals + i));
          __m256i ge_low = _mm256_cmpgt_epi32(v, _mm256_sub_epi32(low_vec, _mm256_set1_epi32(1)));
          __m256i le_high = _mm256_cmpgt_epi32(_mm256_add_epi32(high_vec, _mm256_set1_epi32(1)), v);
          __m256i mask = _mm256_and_si256(ge_low, le_high);
          __m256i masked = _mm256_and_si256(v, mask);
          
          sum_vec = _mm256_add_epi64(sum_vec, _mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked)));
          sum_vec = _mm256_add_epi64(sum_vec, _mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked, 1)));
        }
        
        __m128i sum_low = _mm256_castsi256_si128(sum_vec);
        __m128i sum_high = _mm256_extracti128_si256(sum_vec, 1);
        sum_low = _mm_add_epi64(sum_low, sum_high);
        sum += _mm_extract_epi64(sum_low, 0) + _mm_extract_epi64(sum_low, 1);
        
        // Scalar tail
        for (; i < block_len; ++i) {
          uint32_t v = block_vals[i];
          sum += (v >= low && v <= high) ? v : 0;
        }
      }
    }
  }
  
  return sum;
}