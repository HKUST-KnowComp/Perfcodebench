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
  
  // Precompute adjusted bounds for range check
  // value >= low && value <= high
  // equivalent to: (value - low) <= (high - low) for unsigned
  
  const __m256i base_vec = _mm256_set1_epi32(static_cast<int32_t>(base));
  const __m256i low_vec = _mm256_set1_epi32(static_cast<int32_t>(low));
  const __m256i high_vec = _mm256_set1_epi32(static_cast<int32_t>(high));
  
  for (int iter = 0; iter < iters; ++iter) {
    __m256i sum_lo = _mm256_setzero_si256();
    __m256i sum_hi = _mm256_setzero_si256();
    
    std::size_t i = 0;
    
    // Process 16 elements at a time (256 bits = 16 x 16-bit)
    for (; i + 16 <= n; i += 16) {
      // Load 16 uint16_t values
      __m256i d16 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
      
      // Zero-extend lower 8 uint16_t to uint32_t
      __m256i d32_lo = _mm256_cvtepu16_epi32(_mm256_castsi256_si128(d16));
      // Zero-extend upper 8 uint16_t to uint32_t
      __m256i d32_hi = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(d16, 1));
      
      // Add base to get decoded values
      __m256i val_lo = _mm256_add_epi32(d32_lo, base_vec);
      __m256i val_hi = _mm256_add_epi32(d32_hi, base_vec);
      
      // Range check: value >= low && value <= high
      // Using signed comparison after ensuring values fit in signed range
      // For unsigned comparison: use _mm256_cmpgt_epi32 with offset trick
      // Or use: (val >= low) = !(val < low) = !(low > val)
      //         (val <= high) = !(val > high)
      
      // mask_ge_low: val >= low  =>  !(low > val)
      __m256i cmp_lt_low_lo = _mm256_cmpgt_epi32(low_vec, val_lo);
      __m256i cmp_lt_low_hi = _mm256_cmpgt_epi32(low_vec, val_hi);
      
      // mask_le_high: val <= high  =>  !(val > high)
      __m256i cmp_gt_high_lo = _mm256_cmpgt_epi32(val_lo, high_vec);
      __m256i cmp_gt_high_hi = _mm256_cmpgt_epi32(val_hi, high_vec);
      
      // Combined mask: in_range = !cmp_lt_low && !cmp_gt_high
      // = ~(cmp_lt_low | cmp_gt_high)
      __m256i out_of_range_lo = _mm256_or_si256(cmp_lt_low_lo, cmp_gt_high_lo);
      __m256i out_of_range_hi = _mm256_or_si256(cmp_lt_low_hi, cmp_gt_high_hi);
      
      // Mask values (zero out-of-range values)
      __m256i masked_lo = _mm256_andnot_si256(out_of_range_lo, val_lo);
      __m256i masked_hi = _mm256_andnot_si256(out_of_range_hi, val_hi);
      
      // Accumulate as 64-bit to avoid overflow
      // Unpack to 64-bit and add
      // Lower 4 uint32_t -> 4 uint64_t
      __m256i lo_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked_lo));
      __m256i lo_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked_lo, 1));
      __m256i hi_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked_hi));
      __m256i hi_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked_hi, 1));
      
      sum_lo = _mm256_add_epi64(sum_lo, lo_lo);
      sum_lo = _mm256_add_epi64(sum_lo, lo_hi);
      sum_hi = _mm256_add_epi64(sum_hi, hi_lo);
      sum_hi = _mm256_add_epi64(sum_hi, hi_hi);
    }
    
    // Horizontal sum
    __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
    
    // Extract and sum the 4 uint64_t values
    uint64_t tmp[4];
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(tmp), total);
    sum = tmp[0] + tmp[1] + tmp[2] + tmp[3];
    
    // Handle remaining elements
    for (; i < n; ++i) {
      uint32_t val = base + static_cast<uint32_t>(data[i]);
      if (val >= low && val <= high) {
        sum += val;
      }
    }
  }
  
  return sum;
}