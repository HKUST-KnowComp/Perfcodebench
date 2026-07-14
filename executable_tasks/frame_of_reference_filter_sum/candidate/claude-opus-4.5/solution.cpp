#include "interface.h"

#include <cstdint>
#include <vector>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  uint64_t sum = 0;
  const std::size_t n = deltas.size();
  const uint16_t* __restrict data = deltas.data();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;

#if defined(__AVX2__)
    if (n >= 16) {
      __m256i vbase = _mm256_set1_epi32(static_cast<int32_t>(base));
      __m256i vlow = _mm256_set1_epi32(static_cast<int32_t>(low));
      __m256i vhigh = _mm256_set1_epi32(static_cast<int32_t>(high));
      __m256i vsum0 = _mm256_setzero_si256();
      __m256i vsum1 = _mm256_setzero_si256();

      const std::size_t limit = n - 15;
      for (; i < limit; i += 16) {
        // Load 16 uint16_t values
        __m256i d16 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        
        // Zero-extend to 32-bit: lower 8 and upper 8
        __m256i d32_lo = _mm256_cvtepu16_epi32(_mm256_castsi256_si128(d16));
        __m256i d32_hi = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(d16, 1));
        
        // Add base
        __m256i val_lo = _mm256_add_epi32(d32_lo, vbase);
        __m256i val_hi = _mm256_add_epi32(d32_hi, vbase);
        
        // Range check: value >= low && value <= high
        // For unsigned comparison, use signed comparison with bias trick
        // Or use: (value - low) <= (high - low) as unsigned
        // But simpler: use _mm256_cmpgt_epi32 with adjusted values
        // value >= low: !(value < low) = !(low > value)
        // value <= high: !(value > high)
        
        // Using signed comparison (works if values fit in signed range)
        __m256i ge_low_lo = _mm256_or_si256(
            _mm256_cmpgt_epi32(val_lo, vlow),
            _mm256_cmpeq_epi32(val_lo, vlow));
        __m256i le_high_lo = _mm256_or_si256(
            _mm256_cmpgt_epi32(vhigh, val_lo),
            _mm256_cmpeq_epi32(val_lo, vhigh));
        __m256i mask_lo = _mm256_and_si256(ge_low_lo, le_high_lo);
        
        __m256i ge_low_hi = _mm256_or_si256(
            _mm256_cmpgt_epi32(val_hi, vlow),
            _mm256_cmpeq_epi32(val_hi, vlow));
        __m256i le_high_hi = _mm256_or_si256(
            _mm256_cmpgt_epi32(vhigh, val_hi),
            _mm256_cmpeq_epi32(val_hi, vhigh));
        __m256i mask_hi = _mm256_and_si256(ge_low_hi, le_high_hi);
        
        // Mask values
        __m256i masked_lo = _mm256_and_si256(val_lo, mask_lo);
        __m256i masked_hi = _mm256_and_si256(val_hi, mask_hi);
        
        // Accumulate as 64-bit to avoid overflow
        // Unpack to 64-bit and add
        __m256i lo_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked_lo));
        __m256i lo_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked_lo, 1));
        __m256i hi_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked_hi));
        __m256i hi_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked_hi, 1));
        
        vsum0 = _mm256_add_epi64(vsum0, lo_lo);
        vsum0 = _mm256_add_epi64(vsum0, lo_hi);
        vsum1 = _mm256_add_epi64(vsum1, hi_lo);
        vsum1 = _mm256_add_epi64(vsum1, hi_hi);
      }
      
      // Horizontal sum
      __m256i vsum = _mm256_add_epi64(vsum0, vsum1);
      __m128i sum128 = _mm_add_epi64(
          _mm256_castsi256_si128(vsum),
          _mm256_extracti128_si256(vsum, 1));
      sum128 = _mm_add_epi64(sum128, _mm_srli_si128(sum128, 8));
      sum = static_cast<uint64_t>(_mm_cvtsi128_si64(sum128));
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