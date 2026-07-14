#include "interface.h"

#include <immintrin.h>
#include <cstdint>
#include <cstddef>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  
  const uint16_t* data = deltas.data();
  const size_t n = deltas.size();
  uint64_t sum = 0;
  
  // Precompute adjusted bounds for comparison against deltas directly
  // value = base + delta, so low <= value <= high becomes
  // low - base <= delta <= high - base (with underflow handling)
  
  const __m256i base_vec = _mm256_set1_epi32(static_cast<int32_t>(base));
  const __m256i low_vec = _mm256_set1_epi32(static_cast<int32_t>(low));
  const __m256i high_vec = _mm256_set1_epi32(static_cast<int32_t>(high));
  
  for (int iter = 0; iter < iters; ++iter) {
    __m256i sum_lo = _mm256_setzero_si256();
    __m256i sum_hi = _mm256_setzero_si256();
    
    size_t i = 0;
    
    // Process 16 elements at a time (256 bits / 16 bits = 16 elements)
    for (; i + 16 <= n; i += 16) {
      // Load 16 uint16_t values
      __m256i deltas16 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
      
      // Zero-extend lower 8 uint16 to 8 uint32
      __m256i deltas32_lo = _mm256_cvtepu16_epi32(_mm256_castsi256_si128(deltas16));
      // Zero-extend upper 8 uint16 to 8 uint32
      __m256i deltas32_hi = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(deltas16, 1));
      
      // Compute values = base + delta
      __m256i values_lo = _mm256_add_epi32(base_vec, deltas32_lo);
      __m256i values_hi = _mm256_add_epi32(base_vec, deltas32_hi);
      
      // Compare: value >= low (signed comparison, but values are non-negative)
      // For unsigned comparison: use _mm256_cmpgt_epi32(value, low-1) or
      // use the fact that if high < 2^31, signed comparison works
      // We'll use a trick: (value >= low) && (value <= high)
      // Rewrite as: (value - low) <= (high - low) using unsigned comparison
      // But AVX2 doesn't have unsigned compare, so we use signed with bias
      
      // Alternative: use signed comparison which works if all values < 2^31
      __m256i ge_low_lo = _mm256_cmpgt_epi32(values_lo, _mm256_sub_epi32(low_vec, _mm256_set1_epi32(1)));
      __m256i le_high_lo = _mm256_cmpgt_epi32(high_vec, _mm256_sub_epi32(values_lo, _mm256_set1_epi32(1)));
      __m256i mask_lo = _mm256_and_si256(ge_low_lo, le_high_lo);
      
      __m256i ge_low_hi = _mm256_cmpgt_epi32(values_hi, _mm256_sub_epi32(low_vec, _mm256_set1_epi32(1)));
      __m256i le_high_hi = _mm256_cmpgt_epi32(high_vec, _mm256_sub_epi32(values_hi, _mm256_set1_epi32(1)));
      __m256i mask_hi = _mm256_and_si256(ge_low_hi, le_high_hi);
      
      // Mask out values not in range
      __m256i filtered_lo = _mm256_and_si256(values_lo, mask_lo);
      __m256i filtered_hi = _mm256_and_si256(values_hi, mask_hi);
      
      // Accumulate into 64-bit sums to avoid overflow
      // Extend 32-bit to 64-bit and add
      // Lower 4 of filtered_lo
      __m256i ext_lo_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(filtered_lo));
      // Upper 4 of filtered_lo
      __m256i ext_lo_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(filtered_lo, 1));
      // Lower 4 of filtered_hi
      __m256i ext_hi_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(filtered_hi));
      // Upper 4 of filtered_hi
      __m256i ext_hi_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(filtered_hi, 1));
      
      sum_lo = _mm256_add_epi64(sum_lo, ext_lo_lo);
      sum_lo = _mm256_add_epi64(sum_lo, ext_lo_hi);
      sum_hi = _mm256_add_epi64(sum_hi, ext_hi_lo);
      sum_hi = _mm256_add_epi64(sum_hi, ext_hi_hi);
    }
    
    // Horizontal sum of sum_lo and sum_hi
    __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
    __m128i total_lo = _mm256_castsi256_si128(total);
    __m128i total_hi = _mm256_extracti128_si256(total, 1);
    __m128i total128 = _mm_add_epi64(total_lo, total_hi);
    sum = static_cast<uint64_t>(_mm_extract_epi64(total128, 0)) + 
          static_cast<uint64_t>(_mm_extract_epi64(total128, 1));
    
    // Scalar cleanup for remaining elements
    for (; i < n; ++i) {
      uint32_t value = base + static_cast<uint32_t>(data[i]);
      if (value >= low && value <= high) {
        sum += static_cast<uint64_t>(value);
      }
    }
  }
  
  return sum;
}