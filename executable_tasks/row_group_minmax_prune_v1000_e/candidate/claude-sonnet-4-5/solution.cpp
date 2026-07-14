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
  const size_t n = values.size();
  const size_t num_blocks = (n + block_size - 1) / block_size;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    for (size_t block_idx = 0; block_idx < num_blocks; ++block_idx) {
      const uint32_t block_min = mins[block_idx];
      const uint32_t block_max = maxs[block_idx];
      
      // Skip block if entirely outside range
      if (block_max < low || block_min > high) {
        continue;
      }
      
      const size_t block_start = block_idx * block_size;
      const size_t block_end = (block_start + block_size < n) ? block_start + block_size : n;
      const size_t block_len = block_end - block_start;
      
      const uint32_t* data = values.data() + block_start;
      
      // SIMD processing for blocks that need scanning
      const __m256i low_vec = _mm256_set1_epi32(low);
      const __m256i high_vec = _mm256_set1_epi32(high);
      
      size_t i = 0;
      __m256i sum_vec = _mm256_setzero_si256();
      
      // Process 8 elements at a time with AVX2
      for (; i + 7 < block_len; i += 8) {
        __m256i vals = _mm256_loadu_si256((__m256i*)(data + i));
        __m256i ge_low = _mm256_cmpgt_epi32(vals, _mm256_sub_epi32(low_vec, _mm256_set1_epi32(1)));
        __m256i le_high = _mm256_cmpgt_epi32(_mm256_add_epi32(high_vec, _mm256_set1_epi32(1)), vals);
        __m256i mask = _mm256_and_si256(ge_low, le_high);
        __m256i masked_vals = _mm256_and_si256(vals, mask);
        sum_vec = _mm256_add_epi64(sum_vec, _mm256_and_si256(_mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked_vals)), _mm256_cvtepi32_epi64(_mm256_castsi256_si128(mask))));
        sum_vec = _mm256_add_epi64(sum_vec, _mm256_and_si256(_mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked_vals, 1)), _mm256_cvtepi32_epi64(_mm256_extracti128_si256(mask, 1))));
      }
      
      // Horizontal sum of SIMD accumulator
      uint64_t simd_sum[4];
      _mm256_storeu_si256((__m256i*)simd_sum, sum_vec);
      sum += simd_sum[0] + simd_sum[1] + simd_sum[2] + simd_sum[3];
      
      // Scalar tail
      for (; i < block_len; ++i) {
        uint32_t val = data[i];
        if (val >= low && val <= high) {
          sum += val;
        }
      }
    }
  }
  
  return sum;
}