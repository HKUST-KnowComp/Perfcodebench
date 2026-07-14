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
      __m128i v_current = _mm_setzero_si128();
      __m256i v_offset = _mm256_set1_epi64x(offset);
      __m256i v_sum = _mm256_setzero_si256();
      
      for (size_t i = 0; i < simd_count; i += 4) {
        __m128i deltas_i32 = _mm_loadu_si128((__m128i*)&deltas[i]);
        __m256i deltas_i64 = _mm256_cvtepi32_epi64(deltas_i32);
        
        __m128i d0 = _mm256_castsi256_si128(deltas_i64);
        __m128i d1 = _mm256_extracti128_si256(deltas_i64, 1);
        
        __m128i s0 = _mm_add_epi64(v_current, d0);
        __m128i s1 = _mm_add_epi64(s0, _mm_bsrli_si128(d0, 8));
        __m128i s2 = _mm_add_epi64(s1, d1);
        __m128i s3 = _mm_add_epi64(s2, _mm_bsrli_si128(d1, 8));
        
        __m256i prefix = _mm256_setr_epi64x(
          _mm_extract_epi64(s0, 0),
          _mm_extract_epi64(s1, 0),
          _mm_extract_epi64(s2, 0),
          _mm_extract_epi64(s3, 0)
        );
        
        v_current = _mm_set1_epi64x(_mm_extract_epi64(s3, 0));
        
        __m256i adjusted = _mm256_add_epi64(prefix, v_offset);
        v_sum = _mm256_add_epi64(v_sum, adjusted);
      }
      
      current = _mm_extract_epi64(v_current, 0);
      
      uint64_t temp[4];
      _mm256_storeu_si256((__m256i*)temp, v_sum);
      sum = temp[0] + temp[1] + temp[2] + temp[3];
      
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
      
      for (; i < unroll_count; i += 4) {
        current += (int64_t)deltas[i];
        sum0 += (uint64_t)(current + offset);
        
        current += (int64_t)deltas[i + 1];
        sum1 += (uint64_t)(current + offset);
        
        current += (int64_t)deltas[i + 2];
        sum2 += (uint64_t)(current + offset);
        
        current += (int64_t)deltas[i + 3];
        sum3 += (uint64_t)(current + offset);
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