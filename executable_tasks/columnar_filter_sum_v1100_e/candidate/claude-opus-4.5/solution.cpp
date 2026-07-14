#include "interface.h"

#include <cstdint>
#include <immintrin.h>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  
  const size_t n = keys.size();
  const uint32_t* __restrict keys_ptr = keys.data();
  const uint32_t* __restrict values_ptr = values.data();
  const uint8_t* __restrict flags_ptr = flags.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
#if defined(__AVX2__)
    const __m256i threshold_vec = _mm256_set1_epi32(static_cast<int32_t>(key_threshold));
    const __m256i mask3_vec = _mm256_set1_epi32(3);
    const __m256i zero_vec = _mm256_setzero_si256();
    
    __m256i sum_lo = _mm256_setzero_si256();
    __m256i sum_hi = _mm256_setzero_si256();
    
    size_t i = 0;
    const size_t simd_end = (n >= 8) ? (n - 7) : 0;
    
    for (; i < simd_end; i += 8) {
      // Load 8 flags as bytes, expand to 32-bit
      __m128i flags_8 = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(flags_ptr + i));
      __m256i flags_32 = _mm256_cvtepu8_epi32(flags_8);
      
      // Load 8 keys
      __m256i keys_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(keys_ptr + i));
      
      // Load 8 values
      __m256i values_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(values_ptr + i));
      
      // flags != 0
      __m256i flag_mask = _mm256_cmpgt_epi32(flags_32, zero_vec);
      
      // keys < key_threshold (unsigned comparison via signed trick)
      // For unsigned compare: a < b iff (a ^ 0x80000000) < (b ^ 0x80000000) as signed
      const __m256i sign_bit = _mm256_set1_epi32(static_cast<int32_t>(0x80000000u));
      __m256i keys_signed = _mm256_xor_si256(keys_vec, sign_bit);
      __m256i thresh_signed = _mm256_xor_si256(threshold_vec, sign_bit);
      __m256i lt_mask = _mm256_cmpgt_epi32(thresh_signed, keys_signed);
      
      // (keys & 3) == 0
      __m256i and_result = _mm256_and_si256(keys_vec, mask3_vec);
      __m256i eq_mask = _mm256_cmpeq_epi32(and_result, zero_vec);
      
      // Combine all conditions
      __m256i combined_mask = _mm256_and_si256(flag_mask, _mm256_and_si256(lt_mask, eq_mask));
      
      // Zero out non-qualifying values
      __m256i masked_values = _mm256_and_si256(values_vec, combined_mask);
      
      // Accumulate as 64-bit to avoid overflow
      // Split into low and high 4 elements, zero-extend to 64-bit
      __m128i lo_128 = _mm256_castsi256_si128(masked_values);
      __m128i hi_128 = _mm256_extracti128_si256(masked_values, 1);
      
      __m256i lo_64 = _mm256_cvtepu32_epi64(lo_128);
      __m256i hi_64 = _mm256_cvtepu32_epi64(hi_128);
      
      sum_lo = _mm256_add_epi64(sum_lo, lo_64);
      sum_hi = _mm256_add_epi64(sum_hi, hi_64);
    }
    
    // Horizontal sum of accumulators
    __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
    __m128i total_lo = _mm256_castsi256_si128(total);
    __m128i total_hi = _mm256_extracti128_si256(total, 1);
    __m128i sum_128 = _mm_add_epi64(total_lo, total_hi);
    sum_128 = _mm_add_epi64(sum_128, _mm_srli_si128(sum_128, 8));
    sum = static_cast<uint64_t>(_mm_cvtsi128_si64(sum_128));
    
    // Scalar tail
    for (; i < n; ++i) {
      if (flags_ptr[i] && keys_ptr[i] < key_threshold && ((keys_ptr[i] & 3u) == 0u)) {
        sum += static_cast<uint64_t>(values_ptr[i]);
      }
    }
#else
    // Scalar fallback
    for (size_t i = 0; i < n; ++i) {
      if (flags_ptr[i] && keys_ptr[i] < key_threshold && ((keys_ptr[i] & 3u) == 0u)) {
        sum += static_cast<uint64_t>(values_ptr[i]);
      }
    }
#endif
  }
  
  return sum;
}