#include "interface.h"

#include <immintrin.h>
#include <cstdint>
#include <vector>

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
  // value = base + delta, so low <= value <= high becomes
  // low - base <= delta <= high - base (with underflow handling)
  
  const __m256i base_vec = _mm256_set1_epi32(static_cast<int32_t>(base));
  const __m256i low_vec = _mm256_set1_epi32(static_cast<int32_t>(low));
  const __m256i high_vec = _mm256_set1_epi32(static_cast<int32_t>(high));
  
  for (int iter = 0; iter < iters; ++iter) {
    __m256i sum_lo = _mm256_setzero_si256();
    __m256i sum_hi = _mm256_setzero_si256();
    
    std::size_t i = 0;
    
    // Process 16 uint16_t values at a time
    for (; i + 16 <= n; i += 16) {
      // Load 16 uint16_t values
      __m256i d16 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
      
      // Convert to two vectors of 8 uint32_t each
      __m128i d16_lo = _mm256_castsi256_si128(d16);
      __m128i d16_hi = _mm256_extracti128_si256(d16, 1);
      
      __m256i d32_lo = _mm256_cvtepu16_epi32(d16_lo);
      __m256i d32_hi = _mm256_cvtepu16_epi32(d16_hi);
      
      // Add base to get decoded values
      __m256i val_lo = _mm256_add_epi32(d32_lo, base_vec);
      __m256i val_hi = _mm256_add_epi32(d32_hi, base_vec);
      
      // Range check: value >= low && value <= high
      // For unsigned comparison, use signed comparison trick:
      // (value - low) <= (high - low) as unsigned
      // Or use _mm256_cmpgt_epi32 with bias for unsigned
      
      // Using signed comparison with offset trick for unsigned:
      // val >= low: !(val < low) using unsigned compare
      // val <= high: !(val > high) using unsigned compare
      
      // For AVX2, we can use: val >= low is equivalent to max(val, low) == val
      // val <= high is equivalent to min(val, high) == val
      
      __m256i max_lo_lo = _mm256_max_epu32(val_lo, low_vec);
      __m256i max_lo_hi = _mm256_max_epu32(val_hi, low_vec);
      __m256i min_hi_lo = _mm256_min_epu32(val_lo, high_vec);
      __m256i min_hi_hi = _mm256_min_epu32(val_hi, high_vec);
      
      // val >= low: max(val, low) == val
      __m256i ge_low_lo = _mm256_cmpeq_epi32(max_lo_lo, val_lo);
      __m256i ge_low_hi = _mm256_cmpeq_epi32(max_lo_hi, val_hi);
      
      // val <= high: min(val, high) == val
      __m256i le_high_lo = _mm256_cmpeq_epi32(min_hi_lo, val_lo);
      __m256i le_high_hi = _mm256_cmpeq_epi32(min_hi_hi, val_hi);
      
      // Combined mask
      __m256i mask_lo = _mm256_and_si256(ge_low_lo, le_high_lo);
      __m256i mask_hi = _mm256_and_si256(ge_low_hi, le_high_hi);
      
      // Apply mask to values
      __m256i masked_lo = _mm256_and_si256(val_lo, mask_lo);
      __m256i masked_hi = _mm256_and_si256(val_hi, mask_hi);
      
      // Accumulate as 64-bit to avoid overflow
      // Split each 256-bit vector of 8x32-bit into 4x64-bit sums
      // Use horizontal add pattern
      
      // For masked_lo (8 x 32-bit values), convert to 4 x 64-bit
      __m256i zero = _mm256_setzero_si256();
      
      // Unpack to 64-bit
      __m256i lo_lo = _mm256_unpacklo_epi32(masked_lo, zero);
      __m256i lo_hi = _mm256_unpackhi_epi32(masked_lo, zero);
      __m256i hi_lo = _mm256_unpacklo_epi32(masked_hi, zero);
      __m256i hi_hi = _mm256_unpackhi_epi32(masked_hi, zero);
      
      sum_lo = _mm256_add_epi64(sum_lo, lo_lo);
      sum_lo = _mm256_add_epi64(sum_lo, lo_hi);
      sum_hi = _mm256_add_epi64(sum_hi, hi_lo);
      sum_hi = _mm256_add_epi64(sum_hi, hi_hi);
    }
    
    // Horizontal sum of sum_lo and sum_hi
    __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
    
    // Extract and sum the 4 x 64-bit values
    __m128i total_lo = _mm256_castsi256_si128(total);
    __m128i total_hi = _mm256_extracti128_si256(total, 1);
    __m128i total_128 = _mm_add_epi64(total_lo, total_hi);
    
    uint64_t s0 = static_cast<uint64_t>(_mm_extract_epi64(total_128, 0));
    uint64_t s1 = static_cast<uint64_t>(_mm_extract_epi64(total_128, 1));
    sum = s0 + s1;
    
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