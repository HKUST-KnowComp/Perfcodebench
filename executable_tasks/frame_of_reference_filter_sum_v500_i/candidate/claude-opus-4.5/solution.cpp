#include "interface.h"

#include <cstdint>
#include <immintrin.h>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  
  const uint16_t* data = deltas.data();
  const std::size_t n = deltas.size();
  uint64_t sum = 0;
  
  // Precompute adjusted bounds for range check on deltas directly
  // value = base + delta, low <= value <= high
  // => low - base <= delta <= high - base
  // But we need to handle underflow carefully
  
  const __m256i base_vec = _mm256_set1_epi32(static_cast<int32_t>(base));
  const __m256i low_vec = _mm256_set1_epi32(static_cast<int32_t>(low));
  const __m256i high_vec = _mm256_set1_epi32(static_cast<int32_t>(high));
  
  for (int iter = 0; iter < iters; ++iter) {
    __m256i sum_lo = _mm256_setzero_si256();
    __m256i sum_hi = _mm256_setzero_si256();
    
    std::size_t i = 0;
    
    // Process 16 uint16_t values at a time (256 bits)
    for (; i + 16 <= n; i += 16) {
      // Load 16 uint16_t values
      __m256i d16 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
      
      // Zero-extend lower 8 uint16_t to uint32_t
      __m128i d16_lo = _mm256_castsi256_si128(d16);
      __m128i d16_hi = _mm256_extracti128_si256(d16, 1);
      
      __m256i vals_lo = _mm256_cvtepu16_epi32(d16_lo);
      __m256i vals_hi = _mm256_cvtepu16_epi32(d16_hi);
      
      // Add base
      vals_lo = _mm256_add_epi32(vals_lo, base_vec);
      vals_hi = _mm256_add_epi32(vals_hi, base_vec);
      
      // Range check: low <= value <= high
      // Using unsigned comparison trick: (value - low) <= (high - low)
      // But simpler: compare value >= low AND value <= high
      // For signed comparison with unsigned values, we use the fact that
      // all our values fit in 32 bits and we can use _mm256_cmpgt_epi32
      // after adjusting for signed comparison
      
      // value >= low: NOT(value < low) = NOT(low > value)
      __m256i cmp_ge_low_lo = _mm256_andnot_si256(_mm256_cmpgt_epi32(low_vec, vals_lo), _mm256_set1_epi32(-1));
      __m256i cmp_ge_low_hi = _mm256_andnot_si256(_mm256_cmpgt_epi32(low_vec, vals_hi), _mm256_set1_epi32(-1));
      
      // value <= high: NOT(value > high)
      __m256i cmp_le_high_lo = _mm256_andnot_si256(_mm256_cmpgt_epi32(vals_lo, high_vec), _mm256_set1_epi32(-1));
      __m256i cmp_le_high_hi = _mm256_andnot_si256(_mm256_cmpgt_epi32(vals_hi, high_vec), _mm256_set1_epi32(-1));
      
      // Combined mask
      __m256i mask_lo = _mm256_and_si256(cmp_ge_low_lo, cmp_le_high_lo);
      __m256i mask_hi = _mm256_and_si256(cmp_ge_low_hi, cmp_le_high_hi);
      
      // Apply mask to values
      __m256i masked_lo = _mm256_and_si256(vals_lo, mask_lo);
      __m256i masked_hi = _mm256_and_si256(vals_hi, mask_hi);
      
      // Accumulate as 64-bit to avoid overflow
      // Split each 256-bit register of 8x32-bit into 4x64-bit pairs
      __m256i masked_lo_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked_lo));
      __m256i masked_lo_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked_lo, 1));
      __m256i masked_hi_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked_hi));
      __m256i masked_hi_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked_hi, 1));
      
      sum_lo = _mm256_add_epi64(sum_lo, masked_lo_lo);
      sum_lo = _mm256_add_epi64(sum_lo, masked_lo_hi);
      sum_hi = _mm256_add_epi64(sum_hi, masked_hi_lo);
      sum_hi = _mm256_add_epi64(sum_hi, masked_hi_hi);
    }
    
    // Horizontal sum
    __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
    __m128i total_lo = _mm256_castsi256_si128(total);
    __m128i total_hi = _mm256_extracti128_si256(total, 1);
    __m128i total128 = _mm_add_epi64(total_lo, total_hi);
    total128 = _mm_add_epi64(total128, _mm_srli_si128(total128, 8));
    sum = static_cast<uint64_t>(_mm_cvtsi128_si64(total128));
    
    // Handle remaining elements
    for (; i < n; ++i) {
      uint32_t value = base + static_cast<uint32_t>(data[i]);
      if (value >= low && value <= high) {
        sum += value;
      }
    }
  }
  
  return sum;
}