#include "interface.h"

#include <cstdint>
#include <vector>

#ifdef __AVX2__
#include <immintrin.h>
#endif

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
      
      // Skip blocks completely outside range
      if (block_max < low || block_min > high) {
        continue;
      }
      
      const size_t block_start = block_idx * block_size;
      const size_t block_end = (block_start + block_size < n) ? block_start + block_size : n;
      
      // Fast path: entire block is within range
      if (block_min >= low && block_max <= high) {
        uint64_t block_sum = 0;
        size_t i = block_start;
        
#ifdef __AVX2__
        // SIMD processing for aligned chunks
        const size_t simd_end = block_start + ((block_end - block_start) / 8) * 8;
        __m256i sum_vec = _mm256_setzero_si256();
        
        for (; i < simd_end; i += 8) {
          __m256i vals = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&values[i]));
          sum_vec = _mm256_add_epi64(sum_vec, _mm256_cvtepu32_epi64(_mm256_castsi256_si128(vals)));
          sum_vec = _mm256_add_epi64(sum_vec, _mm256_cvtepu32_epi64(_mm256_extracti128_si256(vals, 1)));
        }
        
        // Horizontal sum
        alignas(32) uint64_t tmp[4];
        _mm256_store_si256(reinterpret_cast<__m256i*>(tmp), sum_vec);
        block_sum += tmp[0] + tmp[1] + tmp[2] + tmp[3];
#endif
        
        // Scalar tail with unrolling
        for (; i + 3 < block_end; i += 4) {
          block_sum += static_cast<uint64_t>(values[i]);
          block_sum += static_cast<uint64_t>(values[i + 1]);
          block_sum += static_cast<uint64_t>(values[i + 2]);
          block_sum += static_cast<uint64_t>(values[i + 3]);
        }
        
        for (; i < block_end; ++i) {
          block_sum += static_cast<uint64_t>(values[i]);
        }
        
        sum += block_sum;
      } else {
        // Partial overlap: check each value
        uint64_t block_sum = 0;
        size_t i = block_start;
        
        // Unrolled loop for better ILP
        for (; i + 3 < block_end; i += 4) {
          uint32_t v0 = values[i];
          uint32_t v1 = values[i + 1];
          uint32_t v2 = values[i + 2];
          uint32_t v3 = values[i + 3];
          
          // Branch-free accumulation
          block_sum += static_cast<uint64_t>(v0) * (v0 >= low && v0 <= high);
          block_sum += static_cast<uint64_t>(v1) * (v1 >= low && v1 <= high);
          block_sum += static_cast<uint64_t>(v2) * (v2 >= low && v2 <= high);
          block_sum += static_cast<uint64_t>(v3) * (v3 >= low && v3 <= high);
        }
        
        for (; i < block_end; ++i) {
          uint32_t v = values[i];
          block_sum += static_cast<uint64_t>(v) * (v >= low && v <= high);
        }
        
        sum += block_sum;
      }
    }
  }
  
  return sum;
}