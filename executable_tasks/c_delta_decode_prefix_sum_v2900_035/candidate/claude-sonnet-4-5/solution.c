#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#endif

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const uint64_t magic = 0x9E3779B1LL;
  
  for (int iter = 0; iter < iters; ++iter) {
    total = 0;
    int64_t current = 0;
    
#if defined(__AVX2__)
    size_t vec_count = count & ~7;
    if (vec_count >= 8) {
      __m256i v_current = _mm256_setzero_si256();
      __m256i v_sum = _mm256_setzero_si256();
      __m256i v_magic = _mm256_set1_epi64x(magic);
      
      for (size_t i = 0; i < vec_count; i += 8) {
        __m128i d0 = _mm_loadu_si128((__m128i*)(deltas + i));
        __m128i d1 = _mm_loadu_si128((__m128i*)(deltas + i + 4));
        __m256i deltas_vec = _mm256_set_m128i(d1, d0);
        __m256i deltas_64 = _mm256_cvtepi32_epi64(_mm256_castsi256_si128(deltas_vec));
        __m256i deltas_64_hi = _mm256_cvtepi32_epi64(_mm256_extracti128_si256(deltas_vec, 1));
        
        __m256i prefix0 = _mm256_add_epi64(v_current, deltas_64);
        v_current = _mm256_permute4x64_epi64(prefix0, 0xFF);
        __m256i scan0 = _mm256_add_epi64(prefix0, _mm256_slli_si256(prefix0, 8));
        scan0 = _mm256_add_epi64(scan0, _mm256_permute2x128_si256(_mm256_setzero_si256(), scan0, 0x03));
        
        __m256i prefix1 = _mm256_add_epi64(v_current, deltas_64_hi);
        v_current = _mm256_permute4x64_epi64(prefix1, 0xFF);
        __m256i scan1 = _mm256_add_epi64(prefix1, _mm256_slli_si256(prefix1, 8));
        scan1 = _mm256_add_epi64(scan1, _mm256_permute2x128_si256(_mm256_setzero_si256(), scan1, 0x03));
        
        scan0 = _mm256_add_epi64(scan0, v_magic);
        scan1 = _mm256_add_epi64(scan1, v_magic);
        v_sum = _mm256_add_epi64(v_sum, scan0);
        v_sum = _mm256_add_epi64(v_sum, scan1);
      }
      
      int64_t temp[4];
      _mm256_storeu_si256((__m256i*)temp, v_sum);
      total += temp[0] + temp[1] + temp[2] + temp[3];
      current = _mm256_extract_epi64(v_current, 3);
    }
    
    for (size_t i = vec_count; i < count; ++i) {
      current += deltas[i];
      total += (uint64_t)(current + magic);
    }
#else
    size_t unroll_count = count & ~3;
    uint64_t acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0;
    
    for (size_t i = 0; i < unroll_count; i += 4) {
      current += deltas[i];
      acc0 += (uint64_t)(current + magic);
      current += deltas[i + 1];
      acc1 += (uint64_t)(current + magic);
      current += deltas[i + 2];
      acc2 += (uint64_t)(current + magic);
      current += deltas[i + 3];
      acc3 += (uint64_t)(current + magic);
    }
    
    total = acc0 + acc1 + acc2 + acc3;
    
    for (size_t i = unroll_count; i < count; ++i) {
      current += deltas[i];
      total += (uint64_t)(current + magic);
    }
#endif
  }
  
  return total;
}