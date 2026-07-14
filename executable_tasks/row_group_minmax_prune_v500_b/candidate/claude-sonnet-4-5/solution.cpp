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
      // Prune blocks using min/max metadata
      if (maxs[block_idx] < low || mins[block_idx] > high) {
        continue;
      }
      
      const size_t block_start = block_idx * block_size;
      const size_t block_end = (block_idx + 1) * block_size;
      const size_t actual_end = block_end < values.size() ? block_end : values.size();
      
      // Check if entire block is within range
      if (mins[block_idx] >= low && maxs[block_idx] <= high) {
        // All values in block are valid, sum without checking
        for (size_t i = block_start; i < actual_end; ++i) {
          sum += static_cast<uint64_t>(values[i]);
        }
      } else {
        // Partial overlap - use SIMD for range checking
        size_t i = block_start;
        const __m256i low_vec = _mm256_set1_epi32(low);
        const __m256i high_vec = _mm256_set1_epi32(high);
        
        // Process 8 elements at a time with AVX2
        for (; i + 8 <= actual_end; i += 8) {
          __m256i vals = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&values[i]));
          __m256i ge_low = _mm256_cmpgt_epi32(vals, _mm256_sub_epi32(low_vec, _mm256_set1_epi32(1)));
          __m256i le_high = _mm256_cmpgt_epi32(_mm256_add_epi32(high_vec, _mm256_set1_epi32(1)), vals);
          __m256i mask = _mm256_and_si256(ge_low, le_high);
          
          // Extract and sum matching values
          alignas(32) uint32_t vals_arr[8];
          alignas(32) int32_t mask_arr[8];
          _mm256_store_si256(reinterpret_cast<__m256i*>(vals_arr), vals);
          _mm256_store_si256(reinterpret_cast<__m256i*>(mask_arr), mask);
          
          for (int j = 0; j < 8; ++j) {
            if (mask_arr[j]) {
              sum += static_cast<uint64_t>(vals_arr[j]);
            }
          }
        }
        
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