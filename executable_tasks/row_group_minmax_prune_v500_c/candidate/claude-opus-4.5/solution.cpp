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
  const size_t num_blocks = mins.size();
  const uint32_t* vptr = values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    for (size_t b = 0; b < num_blocks; ++b) {
      uint32_t block_min = mins[b];
      uint32_t block_max = maxs[b];
      
      // Skip block entirely if outside range
      if (block_max < low || block_min > high) {
        continue;
      }
      
      size_t start = b * block_size;
      size_t end = start + block_size;
      if (end > n) end = n;
      
      // Include entire block if fully within range
      if (block_min >= low && block_max <= high) {
        for (size_t i = start; i < end; ++i) {
          sum += vptr[i];
        }
      } else {
        // Partial overlap - need to check each element
        size_t i = start;
        size_t len = end - start;
        
        // AVX2 vectorized path
        if (len >= 8) {
          __m256i vlow = _mm256_set1_epi32(static_cast<int32_t>(low));
          __m256i vhigh = _mm256_set1_epi32(static_cast<int32_t>(high));
          __m256i acc_lo = _mm256_setzero_si256();
          __m256i acc_hi = _mm256_setzero_si256();
          
          size_t simd_end = start + (len & ~7ULL);
          
          for (; i < simd_end; i += 8) {
            __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(vptr + i));
            
            // Unsigned comparison: v >= low && v <= high
            // For unsigned: a >= b is equivalent to max(a,b) == a
            // For unsigned: a <= b is equivalent to min(a,b) == a
            __m256i ge_low = _mm256_cmpeq_epi32(_mm256_max_epu32(v, vlow), v);
            __m256i le_high = _mm256_cmpeq_epi32(_mm256_min_epu32(v, vhigh), v);
            __m256i mask = _mm256_and_si256(ge_low, le_high);
            
            __m256i masked = _mm256_and_si256(v, mask);
            
            // Extend to 64-bit and accumulate
            __m256i lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked));
            __m256i hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked, 1));
            acc_lo = _mm256_add_epi64(acc_lo, lo);
            acc_hi = _mm256_add_epi64(acc_hi, hi);
          }
          
          // Horizontal sum
          __m256i acc = _mm256_add_epi64(acc_lo, acc_hi);
          __m128i lo128 = _mm256_castsi256_si128(acc);
          __m128i hi128 = _mm256_extracti128_si256(acc, 1);
          __m128i sum128 = _mm_add_epi64(lo128, hi128);
          sum += static_cast<uint64_t>(_mm_extract_epi64(sum128, 0)) + 
                 static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));
        }
        
        // Scalar tail
        for (; i < end; ++i) {
          uint32_t v = vptr[i];
          if (v >= low && v <= high) {
            sum += v;
          }
        }
      }
    }
  }
  return sum;
}