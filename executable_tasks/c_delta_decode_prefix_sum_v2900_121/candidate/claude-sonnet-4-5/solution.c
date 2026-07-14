#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

#ifdef __AVX2__
#include <immintrin.h>
#endif

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const int64_t offset = 0x9E3779B1LL;
  
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t sum = 0;
    
#ifdef __AVX2__
    if (count >= 4) {
      size_t simd_count = count & ~3ULL;
      __m256i v_current = _mm256_setzero_si256();
      __m256i v_offset = _mm256_set1_epi64x(offset);
      __m256i v_sum_lo = _mm256_setzero_si256();
      __m256i v_sum_hi = _mm256_setzero_si256();
      
      for (size_t i = 0; i < simd_count; i += 4) {
        __m128i deltas_32 = _mm_loadu_si128((__m128i*)&deltas[i]);
        __m256i deltas_64 = _mm256_cvtepi32_epi64(deltas_32);
        
        __m256i prefix0 = deltas_64;
        __m256i prefix1 = _mm256_add_epi64(prefix0, _mm256_bslli_epi128(prefix0, 8));
        __m256i prefix2 = _mm256_add_epi64(prefix1, _mm256_permute2x128_si256(_mm256_setzero_si256(), prefix1, 0x03));
        
        v_current = _mm256_add_epi64(v_current, prefix2);
        __m256i values = _mm256_add_epi64(v_current, v_offset);
        
        __m256i values_lo = _mm256_and_si256(values, _mm256_set1_epi64x(0xFFFFFFFFULL));
        __m256i values_hi = _mm256_srli_epi64(values, 32);
        v_sum_lo = _mm256_add_epi64(v_sum_lo, values_lo);
        v_sum_hi = _mm256_add_epi64(v_sum_hi, values_hi);
        
        v_current = _mm256_permute4x64_epi64(v_current, 0xFF);
      }
      
      uint64_t sums_lo[4], sums_hi[4];
      _mm256_storeu_si256((__m256i*)sums_lo, v_sum_lo);
      _mm256_storeu_si256((__m256i*)sums_hi, v_sum_hi);
      
      for (int j = 0; j < 4; ++j) {
        sum += sums_lo[j] + (sums_hi[j] << 32);
      }
      
      current = _mm256_extract_epi64(v_current, 0);
      
      for (size_t i = simd_count; i < count; ++i) {
        current += (int64_t)deltas[i];
        sum += (uint64_t)(current + offset);
      }
    } else
#endif
    {
      size_t i = 0;
      size_t unroll_count = count & ~3ULL;
      
      uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
      int64_t c0 = 0, c1 = 0, c2 = 0, c3 = 0;
      
      for (; i < unroll_count; i += 4) {
        c0 = current + (int64_t)deltas[i];
        c1 = c0 + (int64_t)deltas[i + 1];
        c2 = c1 + (int64_t)deltas[i + 2];
        c3 = c2 + (int64_t)deltas[i + 3];
        
        sum0 += (uint64_t)(c0 + offset);
        sum1 += (uint64_t)(c1 + offset);
        sum2 += (uint64_t)(c2 + offset);
        sum3 += (uint64_t)(c3 + offset);
        
        current = c3;
      }
      
      sum = sum0 + sum1 + sum2 + sum3;
      
      for (; i < count; ++i) {
        current += (int64_t)deltas[i];
        sum += (uint64_t)(current + offset);
      }
    }
    
    total = sum;
  }
  
  return total;
}