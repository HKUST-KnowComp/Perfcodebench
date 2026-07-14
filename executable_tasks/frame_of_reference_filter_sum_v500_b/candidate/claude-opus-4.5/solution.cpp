#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>
#define HAS_SSE4 1
#if defined(__AVX2__)
#define HAS_AVX2 1
#else
#define HAS_AVX2 0
#endif
#else
#define HAS_SSE4 0
#define HAS_AVX2 0
#endif

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  
  const size_t n = deltas.size();
  const uint16_t* __restrict data = deltas.data();
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t i = 0;
    
#if HAS_AVX2
    // AVX2 path: process 16 elements at a time
    if (n >= 16) {
      __m256i vbase = _mm256_set1_epi32(static_cast<int32_t>(base));
      __m256i vlow = _mm256_set1_epi32(static_cast<int32_t>(low));
      __m256i vhigh = _mm256_set1_epi32(static_cast<int32_t>(high));
      __m256i vsum0 = _mm256_setzero_si256();
      __m256i vsum1 = _mm256_setzero_si256();
      
      size_t vec_end = n - (n % 16);
      for (; i < vec_end; i += 16) {
        // Load 16 uint16_t values
        __m256i raw = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        
        // Zero-extend to 32-bit: lower 8 and upper 8
        __m256i vals0 = _mm256_cvtepu16_epi32(_mm256_castsi256_si128(raw));
        __m256i vals1 = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(raw, 1));
        
        // Add base
        __m256i decoded0 = _mm256_add_epi32(vals0, vbase);
        __m256i decoded1 = _mm256_add_epi32(vals1, vbase);
        
        // Compare: value >= low && value <= high
        // For unsigned comparison, we use signed comparison with offset trick
        // Or use _mm256_cmpgt_epi32 with adjusted values
        // Actually for this range, we can use: (value >= low) && (value <= high)
        // Using signed comparison works if values don't exceed INT32_MAX
        __m256i ge_low0 = _mm256_or_si256(
            _mm256_cmpgt_epi32(decoded0, vlow),
            _mm256_cmpeq_epi32(decoded0, vlow));
        __m256i le_high0 = _mm256_or_si256(
            _mm256_cmpgt_epi32(vhigh, decoded0),
            _mm256_cmpeq_epi32(decoded0, vhigh));
        __m256i mask0 = _mm256_and_si256(ge_low0, le_high0);
        
        __m256i ge_low1 = _mm256_or_si256(
            _mm256_cmpgt_epi32(decoded1, vlow),
            _mm256_cmpeq_epi32(decoded1, vlow));
        __m256i le_high1 = _mm256_or_si256(
            _mm256_cmpgt_epi32(vhigh, decoded1),
            _mm256_cmpeq_epi32(decoded1, vhigh));
        __m256i mask1 = _mm256_and_si256(ge_low1, le_high1);
        
        // Mask and accumulate
        __m256i masked0 = _mm256_and_si256(decoded0, mask0);
        __m256i masked1 = _mm256_and_si256(decoded1, mask1);
        
        vsum0 = _mm256_add_epi64(vsum0, _mm256_add_epi64(
            _mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked0)),
            _mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked0, 1))));
        vsum1 = _mm256_add_epi64(vsum1, _mm256_add_epi64(
            _mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked1)),
            _mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked1, 1))));
      }
      
      // Horizontal sum
      __m256i vtotal = _mm256_add_epi64(vsum0, vsum1);
      __m128i lo = _mm256_castsi256_si128(vtotal);
      __m128i hi = _mm256_extracti128_si256(vtotal, 1);
      __m128i sum128 = _mm_add_epi64(lo, hi);
      sum = static_cast<uint64_t>(_mm_extract_epi64(sum128, 0)) + 
            static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));
    }
#elif HAS_SSE4
    // SSE4.1 path: process 8 elements at a time
    if (n >= 8) {
      __m128i vbase = _mm_set1_epi32(static_cast<int32_t>(base));
      __m128i vlow = _mm_set1_epi32(static_cast<int32_t>(low));
      __m128i vhigh = _mm_set1_epi32(static_cast<int32_t>(high));
      __m128i vsum0 = _mm_setzero_si128();
      __m128i vsum1 = _mm_setzero_si128();
      
      size_t vec_end = n - (n % 8);
      for (; i < vec_end; i += 8) {
        // Load 8 uint16_t values
        __m128i raw = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
        
        // Zero-extend to 32-bit
        __m128i vals0 = _mm_cvtepu16_epi32(raw);
        __m128i vals1 = _mm_cvtepu16_epi32(_mm_srli_si128(raw, 8));
        
        // Add base
        __m128i decoded0 = _mm_add_epi32(vals0, vbase);
        __m128i decoded1 = _mm_add_epi32(vals1, vbase);
        
        // Compare for range
        __m128i ge_low0 = _mm_or_si128(
            _mm_cmpgt_epi32(decoded0, vlow),
            _mm_cmpeq_epi32(decoded0, vlow));
        __m128i le_high0 = _mm_or_si128(
            _mm_cmpgt_epi32(vhigh, decoded0),
            _mm_cmpeq_epi32(decoded0, vhigh));
        __m128i mask0 = _mm_and_si128(ge_low0, le_high0);
        
        __m128i ge_low1 = _mm_or_si128(
            _mm_cmpgt_epi32(decoded1, vlow),
            _mm_cmpeq_epi32(decoded1, vlow));
        __m128i le_high1 = _mm_or_si128(
            _mm_cmpgt_epi32(vhigh, decoded1),
            _mm_cmpeq_epi32(decoded1, vhigh));
        __m128i mask1 = _mm_and_si128(ge_low1, le_high1);
        
        // Mask and accumulate to 64-bit
        __m128i masked0 = _mm_and_si128(decoded0, mask0);
        __m128i masked1 = _mm_and_si128(decoded1, mask1);
        
        vsum0 = _mm_add_epi64(vsum0, _mm_add_epi64(
            _mm_cvtepu32_epi64(masked0),
            _mm_cvtepu32_epi64(_mm_srli_si128(masked0, 8))));
        vsum1 = _mm_add_epi64(vsum1, _mm_add_epi64(
            _mm_cvtepu32_epi64(masked1),
            _mm_cvtepu32_epi64(_mm_srli_si128(masked1, 8))));
      }
      
      // Horizontal sum
      __m128i vtotal = _mm_add_epi64(vsum0, vsum1);
      sum = static_cast<uint64_t>(_mm_extract_epi64(vtotal, 0)) + 
            static_cast<uint64_t>(_mm_extract_epi64(vtotal, 1));
    }
#endif
    
    // Scalar tail
    for (; i < n; ++i) {
      uint32_t value = base + static_cast<uint32_t>(data[i]);
      if (value >= low && value <= high) {
        sum += value;
      }
    }
  }
  
  return sum;
}