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

  // Precompute adjusted bounds to avoid per-element base addition in comparisons
  // value = base + delta, low <= value <= high
  // => low - base <= delta <= high - base (with underflow handling)
  
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
      
      // Zero-extend lower 8 uint16 to uint32
      __m256i d32_lo = _mm256_cvtepu16_epi32(_mm256_castsi256_si128(d16));
      // Zero-extend upper 8 uint16 to uint32
      __m256i d32_hi = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(d16, 1));
      
      // Add base to get decoded values
      __m256i val_lo = _mm256_add_epi32(d32_lo, base_vec);
      __m256i val_hi = _mm256_add_epi32(d32_hi, base_vec);
      
      // Compare: value >= low (signed comparison, but values are non-negative)
      // For unsigned comparison: val >= low iff (val - low) doesn't underflow
      // Use signed comparison which works for values < 2^31
      __m256i ge_low_lo = _mm256_or_si256(
          _mm256_cmpgt_epi32(val_lo, low_vec),
          _mm256_cmpeq_epi32(val_lo, low_vec));
      __m256i ge_low_hi = _mm256_or_si256(
          _mm256_cmpgt_epi32(val_hi, low_vec),
          _mm256_cmpeq_epi32(val_hi, low_vec));
      
      // Compare: value <= high iff high >= value
      __m256i le_high_lo = _mm256_or_si256(
          _mm256_cmpgt_epi32(high_vec, val_lo),
          _mm256_cmpeq_epi32(high_vec, val_lo));
      __m256i le_high_hi = _mm256_or_si256(
          _mm256_cmpgt_epi32(high_vec, val_hi),
          _mm256_cmpeq_epi32(high_vec, val_hi));
      
      // Combined mask: in range
      __m256i mask_lo = _mm256_and_si256(ge_low_lo, le_high_lo);
      __m256i mask_hi = _mm256_and_si256(ge_low_hi, le_high_hi);
      
      // Mask values (zero out those not in range)
      __m256i masked_lo = _mm256_and_si256(val_lo, mask_lo);
      __m256i masked_hi = _mm256_and_si256(val_hi, mask_hi);
      
      // Accumulate as 64-bit to avoid overflow
      // Unpack uint32 to uint64 and add
      __m256i zero = _mm256_setzero_si256();
      
      // For masked_lo: 8 x uint32 -> 4 x uint64 (low) + 4 x uint64 (high)
      __m256i lo_lo = _mm256_unpacklo_epi32(masked_lo, zero);
      __m256i lo_hi = _mm256_unpackhi_epi32(masked_lo, zero);
      sum_lo = _mm256_add_epi64(sum_lo, lo_lo);
      sum_hi = _mm256_add_epi64(sum_hi, lo_hi);
      
      // For masked_hi: 8 x uint32 -> 4 x uint64 (low) + 4 x uint64 (high)
      __m256i hi_lo = _mm256_unpacklo_epi32(masked_hi, zero);
      __m256i hi_hi = _mm256_unpackhi_epi32(masked_hi, zero);
      sum_lo = _mm256_add_epi64(sum_lo, hi_lo);
      sum_hi = _mm256_add_epi64(sum_hi, hi_hi);
    }
    
    // Horizontal sum of sum_lo and sum_hi
    __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
    // total contains 4 x uint64
    __m128i t128_lo = _mm256_castsi256_si128(total);
    __m128i t128_hi = _mm256_extracti128_si256(total, 1);
    __m128i t128 = _mm_add_epi64(t128_lo, t128_hi);
    // t128 contains 2 x uint64
    uint64_t s0 = static_cast<uint64_t>(_mm_extract_epi64(t128, 0));
    uint64_t s1 = static_cast<uint64_t>(_mm_extract_epi64(t128, 1));
    sum = s0 + s1;
    
    // Handle remainder with scalar code
    for (; i < n; ++i) {
      uint32_t value = base + static_cast<uint32_t>(data[i]);
      if (value >= low && value <= high) {
        sum += static_cast<uint64_t>(value);
      }
    }
  }
  
  return sum;
}