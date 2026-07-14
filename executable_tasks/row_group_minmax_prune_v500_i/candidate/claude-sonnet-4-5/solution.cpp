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
  const uint32_t* val_ptr = values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    for (size_t block_idx = 0; block_idx < num_blocks; ++block_idx) {
      const uint32_t block_min = mins[block_idx];
      const uint32_t block_max = maxs[block_idx];
      
      // Skip blocks completely outside range
      if (block_max < low || block_min > high) {
        continue;
      }
      
      const size_t block_start = block_idx * block_size;
      const size_t block_end = block_start + block_size;
      const uint32_t* block_ptr = val_ptr + block_start;
      
      // Prefetch next block metadata
      if (block_idx + 1 < num_blocks) {
        __builtin_prefetch(&mins[block_idx + 1], 0, 1);
        __builtin_prefetch(&maxs[block_idx + 1], 0, 1);
      }
      
      // If entire block is within range, use fast path
      if (block_min >= low && block_max <= high) {
        // SIMD summation for fully-contained blocks
        size_t i = 0;
        __m256i sum_vec = _mm256_setzero_si256();
        
        for (; i + 8 <= block_size; i += 8) {
          __m256i vals = _mm256_loadu_si256((__m256i*)(block_ptr + i));
          sum_vec = _mm256_add_epi32(sum_vec, vals);
        }
        
        // Horizontal sum
        __m128i sum_low = _mm256_castsi256_si128(sum_vec);
        __m128i sum_high = _mm256_extracti128_si256(sum_vec, 1);
        __m128i sum128 = _mm_add_epi32(sum_low, sum_high);
        sum128 = _mm_hadd_epi32(sum128, sum128);
        sum128 = _mm_hadd_epi32(sum128, sum128);
        sum += _mm_cvtsi128_si32(sum128);
        
        // Scalar tail
        for (; i < block_size; ++i) {
          sum += block_ptr[i];
        }
      } else {
        // Partial overlap: check each value with unrolling
        size_t i = 0;
        uint64_t local_sum = 0;
        
        for (; i + 4 <= block_size; i += 4) {
          uint32_t v0 = block_ptr[i];
          uint32_t v1 = block_ptr[i + 1];
          uint32_t v2 = block_ptr[i + 2];
          uint32_t v3 = block_ptr[i + 3];
          
          if (v0 >= low && v0 <= high) local_sum += v0;
          if (v1 >= low && v1 <= high) local_sum += v1;
          if (v2 >= low && v2 <= high) local_sum += v2;
          if (v3 >= low && v3 <= high) local_sum += v3;
        }
        
        for (; i < block_size; ++i) {
          uint32_t v = block_ptr[i];
          if (v >= low && v <= high) {
            local_sum += v;
          }
        }
        
        sum += local_sum;
      }
    }
  }
  
  return sum;
}