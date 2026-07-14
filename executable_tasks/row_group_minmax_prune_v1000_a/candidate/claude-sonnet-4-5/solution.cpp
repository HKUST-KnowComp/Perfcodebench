#include "interface.h"

#include <cstdint>
#include <vector>

#ifdef __SSE2__
#include <emmintrin.h>
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
  const uint32_t* vals = values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    for (size_t block_idx = 0; block_idx < num_blocks; ++block_idx) {
      // Check min/max metadata for pruning
      uint32_t block_min = mins[block_idx];
      uint32_t block_max = maxs[block_idx];
      
      // Skip block if entirely outside range
      if (block_max < low || block_min > high) {
        continue;
      }
      
      size_t start = block_idx * block_size;
      size_t end = (start + block_size < n) ? start + block_size : n;
      
      // If entire block is within range, sum without checks
      if (block_min >= low && block_max <= high) {
        size_t i = start;
        
#ifdef __SSE2__
        // SIMD path for aligned processing
        __m128i sum_vec = _mm_setzero_si128();
        for (; i + 4 <= end; i += 4) {
          __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&vals[i]));
          __m128i v_lo = _mm_unpacklo_epi32(v, _mm_setzero_si128());
          __m128i v_hi = _mm_unpackhi_epi32(v, _mm_setzero_si128());
          sum_vec = _mm_add_epi64(sum_vec, v_lo);
          sum_vec = _mm_add_epi64(sum_vec, v_hi);
        }
        
        uint64_t tmp[2];
        _mm_storeu_si128(reinterpret_cast<__m128i*>(tmp), sum_vec);
        sum += tmp[0] + tmp[1];
#endif
        
        // Scalar tail
        for (; i < end; ++i) {
          sum += vals[i];
        }
      } else {
        // Partial overlap: check each value with unrolling
        size_t i = start;
        
        // Unroll by 4
        for (; i + 4 <= end; i += 4) {
          uint32_t v0 = vals[i];
          uint32_t v1 = vals[i + 1];
          uint32_t v2 = vals[i + 2];
          uint32_t v3 = vals[i + 3];
          
          if (v0 >= low && v0 <= high) sum += v0;
          if (v1 >= low && v1 <= high) sum += v1;
          if (v2 >= low && v2 <= high) sum += v2;
          if (v3 >= low && v3 <= high) sum += v3;
        }
        
        // Tail
        for (; i < end; ++i) {
          uint32_t v = vals[i];
          if (v >= low && v <= high) {
            sum += v;
          }
        }
      }
    }
  }
  
  return sum;
}