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
      
      // Zero-extend lower 8 uint16_t to uint32_t and add base
      __m256i vals_lo = _mm256_add_epi32(_mm256_cvtepu16_epi32(_mm256_castsi256_si128(d16)), base_vec);
      // Zero-extend upper 8 uint16_t to uint32_t and add base
      __m256i vals_hi = _mm256_add_epi32(_mm256_cvtepu16_epi32(_mm256_extracti128_si256(d16, 1)), base_vec);
      
      // Range check: value >= low && value <= high
      // Using signed comparison after ensuring values fit in signed range
      // For unsigned: use _mm256_cmpgt_epi32 with offset trick or separate comparisons
      
      // mask_ge_low: value >= low  =>  NOT(value < low)  =>  NOT(low > value)
      // mask_le_high: value <= high  =>  NOT(value > high)
      
      // For unsigned comparison, we use the fact that:
      // a >= b (unsigned) is equivalent to: max(a,b) == a
      // a <= b (unsigned) is equivalent to: min(a,b) == a
      
      __m256i max_lo = _mm256_max_epu32(vals_lo, low_vec);
      __m256i min_lo = _mm256_min_epu32(vals_lo, high_vec);
      __m256i mask_ge_low_lo = _mm256_cmpeq_epi32(max_lo, vals_lo);
      __m256i mask_le_high_lo = _mm256_cmpeq_epi32(min_lo, vals_lo);
      __m256i mask_lo = _mm256_and_si256(mask_ge_low_lo, mask_le_high_lo);
      
      __m256i max_hi = _mm256_max_epu32(vals_hi, low_vec);
      __m256i min_hi = _mm256_min_epu32(vals_hi, high_vec);
      __m256i mask_ge_low_hi = _mm256_cmpeq_epi32(max_hi, vals_hi);
      __m256i mask_le_high_hi = _mm256_cmpeq_epi32(min_hi, vals_hi);
      __m256i mask_hi = _mm256_and_si256(mask_ge_low_hi, mask_le_high_hi);
      
      // Apply mask and accumulate (masked values become 0)
      __m256i masked_lo = _mm256_and_si256(vals_lo, mask_lo);
      __m256i masked_hi = _mm256_and_si256(vals_hi, mask_hi);
      
      // Accumulate into 64-bit to avoid overflow
      // Unpack to 64-bit and add
      // vals are 32-bit, we need to accumulate as 64-bit
      
      // For masked_lo: 8 x 32-bit values
      // Split into 4 x 64-bit by zero-extending
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
    
    // Sum the 4 x 64-bit values in total
    __m128i t_lo = _mm256_castsi256_si128(total);
    __m128i t_hi = _mm256_extracti128_si256(total, 1);
    __m128i t_sum = _mm_add_epi64(t_lo, t_hi);
    
    uint64_t result = static_cast<uint64_t>(_mm_extract_epi64(t_sum, 0)) + 
                      static_cast<uint64_t>(_mm_extract_epi64(t_sum, 1));
    
    // Handle remaining elements with scalar code
    for (; i < n; ++i) {
      uint32_t value = base + static_cast<uint32_t>(data[i]);
      if (value >= low && value <= high) {
        result += value;
      }
    }
    
    sum = result;
  }
  
  return sum;
}