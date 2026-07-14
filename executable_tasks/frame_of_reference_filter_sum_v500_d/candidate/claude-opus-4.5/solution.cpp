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
  
  // Precompute adjusted bounds for range check
  // value >= low && value <= high
  // equivalent to: (value - low) <= (high - low) for unsigned
  const uint32_t range = high - low;
  
  const __m256i base_vec = _mm256_set1_epi32(static_cast<int32_t>(base));
  const __m256i low_vec = _mm256_set1_epi32(static_cast<int32_t>(low));
  const __m256i range_vec = _mm256_set1_epi32(static_cast<int32_t>(range));
  
  for (int iter = 0; iter < iters; ++iter) {
    __m256i sum_lo = _mm256_setzero_si256();
    __m256i sum_hi = _mm256_setzero_si256();
    
    std::size_t i = 0;
    
    // Process 16 elements at a time (256 bits of uint16_t)
    for (; i + 16 <= n; i += 16) {
      // Load 16 uint16_t values
      __m256i d16 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
      
      // Zero-extend lower 8 uint16_t to uint32_t
      __m128i d16_lo = _mm256_castsi256_si128(d16);
      __m256i d32_lo = _mm256_cvtepu16_epi32(d16_lo);
      
      // Zero-extend upper 8 uint16_t to uint32_t
      __m128i d16_hi = _mm256_extracti128_si256(d16, 1);
      __m256i d32_hi = _mm256_cvtepu16_epi32(d16_hi);
      
      // Add base to get decoded values
      __m256i val_lo = _mm256_add_epi32(d32_lo, base_vec);
      __m256i val_hi = _mm256_add_epi32(d32_hi, base_vec);
      
      // Range check: (value - low) <= range (unsigned comparison)
      // For unsigned comparison, we use: ~(a > b) which is a <= b
      // _mm256_cmpgt_epi32 does signed comparison, so we need a different approach
      // Use: value >= low && value <= high
      // Subtract low, then check if result <= range using max trick
      
      __m256i shifted_lo = _mm256_sub_epi32(val_lo, low_vec);
      __m256i shifted_hi = _mm256_sub_epi32(val_hi, low_vec);
      
      // For unsigned <= comparison: (a <= b) iff (max(a,b) == b) iff (min(a,b) == a)
      // mask where shifted <= range (unsigned)
      __m256i min_lo = _mm256_min_epu32(shifted_lo, range_vec);
      __m256i min_hi = _mm256_min_epu32(shifted_hi, range_vec);
      
      __m256i mask_lo = _mm256_cmpeq_epi32(min_lo, shifted_lo);
      __m256i mask_hi = _mm256_cmpeq_epi32(min_hi, shifted_hi);
      
      // Apply mask to values
      __m256i masked_lo = _mm256_and_si256(val_lo, mask_lo);
      __m256i masked_hi = _mm256_and_si256(val_hi, mask_hi);
      
      // Accumulate as 64-bit to avoid overflow
      // Unpack to 64-bit and add
      // For masked_lo: extract lower and upper 128-bit lanes, then unpack
      __m256i zero = _mm256_setzero_si256();
      
      // Unpack lower 4 uint32 to 4 uint64
      __m256i lo_lo = _mm256_unpacklo_epi32(masked_lo, zero);
      __m256i lo_hi = _mm256_unpackhi_epi32(masked_lo, zero);
      sum_lo = _mm256_add_epi64(sum_lo, lo_lo);
      sum_hi = _mm256_add_epi64(sum_hi, lo_hi);
      
      __m256i hi_lo = _mm256_unpacklo_epi32(masked_hi, zero);
      __m256i hi_hi = _mm256_unpackhi_epi32(masked_hi, zero);
      sum_lo = _mm256_add_epi64(sum_lo, hi_lo);
      sum_hi = _mm256_add_epi64(sum_hi, hi_hi);
    }
    
    // Horizontal sum of sum_lo and sum_hi
    __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
    __m128i total_lo = _mm256_castsi256_si128(total);
    __m128i total_hi = _mm256_extracti128_si256(total, 1);
    __m128i sum128 = _mm_add_epi64(total_lo, total_hi);
    sum = static_cast<uint64_t>(_mm_extract_epi64(sum128, 0)) + 
          static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));
    
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