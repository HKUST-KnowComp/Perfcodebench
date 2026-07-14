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
  const size_t num_blocks = mins.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    for (size_t block_idx = 0; block_idx < num_blocks; ++block_idx) {
      // Skip blocks that are entirely outside the range
      if (maxs[block_idx] < low || mins[block_idx] > high) {
        continue;
      }
      
      const size_t block_start = block_idx * block_size;
      const size_t block_end = (block_idx + 1) * block_size;
      const size_t actual_end = block_end < values.size() ? block_end : values.size();
      
      // If entire block is within range, sum without checks
      if (mins[block_idx] >= low && maxs[block_idx] <= high) {
        size_t i = block_start;
        
        // SIMD processing for aligned chunks
        const size_t simd_end = block_start + ((actual_end - block_start) / 8) * 8;
        __m256i sum_vec = _mm256_setzero_si256();
        
        for (; i < simd_end; i += 8) {
          __m256i vals = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&values[i]));
          sum_vec = _mm256_add_epi64(sum_vec, _mm256_cvtepu32_epi64(_mm256_castsi256_si128(vals)));
          sum_vec = _mm256_add_epi64(sum_vec, _mm256_cvtepu32_epi64(_mm256_extracti128_si256(vals, 1)));
        }
        
        // Extract and sum SIMD results
        uint64_t simd_sum[4];
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(simd_sum), sum_vec);
        sum += simd_sum[0] + simd_sum[1] + simd_sum[2] + simd_sum[3];
        
        // Handle remaining elements
        for (; i < actual_end; ++i) {
          sum += static_cast<uint64_t>(values[i]);
        }
      } else {
        // Block partially overlaps range - need to check each value
        size_t i = block_start;
        
        // SIMD processing with range checks
        const size_t simd_end = block_start + ((actual_end - block_start) / 8) * 8;
        __m256i low_vec = _mm256_set1_epi32(low);
        __m256i high_vec = _mm256_set1_epi32(high);
        __m256i sum_vec = _mm256_setzero_si256();
        
        for (; i < simd_end; i += 8) {
          __m256i vals = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&values[i]));
          __m256i ge_low = _mm256_cmpgt_epi32(vals, _mm256_sub_epi32(low_vec, _mm256_set1_epi32(1)));
          __m256i le_high = _mm256_cmpgt_epi32(_mm256_add_epi32(high_vec, _mm256_set1_epi32(1)), vals);
          __m256i mask = _mm256_and_si256(ge_low, le_high);
          __m256i masked_vals = _mm256_and_si256(vals, mask);
          
          sum_vec = _mm256_add_epi64(sum_vec, _mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked_vals)));
          sum_vec = _mm256_add_epi64(sum_vec, _mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked_vals, 1)));
        }
        
        uint64_t simd_sum[4];
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(simd_sum), sum_vec);
        sum += simd_sum[0] + simd_sum[1] + simd_sum[2] + simd_sum[3];
        
        // Handle remaining elements
        for (; i < actual_end; ++i) {
          uint32_t value = values[i];
          if (value >= low && value <= high) {
            sum += static_cast<uint64_t>(value);
          }
        }
      }
    }
  }
  
  return sum;
}