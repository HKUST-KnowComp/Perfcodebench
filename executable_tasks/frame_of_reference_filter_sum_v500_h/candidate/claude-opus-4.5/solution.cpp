#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>
#define HAS_SSE4 1
#else
#define HAS_SSE4 0
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
    
#if HAS_SSE4
    if (n >= 8) {
      __m128i vbase = _mm_set1_epi32(static_cast<int>(base));
      __m128i vlow = _mm_set1_epi32(static_cast<int>(low));
      __m128i vhigh = _mm_set1_epi32(static_cast<int>(high));
      __m128i vsum_lo = _mm_setzero_si128();
      __m128i vsum_hi = _mm_setzero_si128();
      
      size_t vec_end = (n / 8) * 8;
      
      for (; i < vec_end; i += 8) {
        // Load 8 uint16_t values
        __m128i d16 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
        
        // Zero-extend to 32-bit: lower 4 and upper 4
        __m128i d32_lo = _mm_cvtepu16_epi32(d16);
        __m128i d32_hi = _mm_cvtepu16_epi32(_mm_srli_si128(d16, 8));
        
        // Add base
        __m128i val_lo = _mm_add_epi32(d32_lo, vbase);
        __m128i val_hi = _mm_add_epi32(d32_hi, vbase);
        
        // Compare: val >= low  =>  !(val < low)  =>  low <= val
        // For unsigned comparison, we use signed comparison with offset trick
        // Or use: val >= low is equivalent to (val - low) doesn't underflow in unsigned
        // SSE4.1 doesn't have unsigned compare, so we use signed with bias
        // Actually for range [low, high], we can do: (val - low) <= (high - low) unsigned
        // But simpler: use signed comparison after biasing by 0x80000000
        
        const __m128i bias = _mm_set1_epi32(static_cast<int>(0x80000000u));
        __m128i val_lo_biased = _mm_xor_si128(val_lo, bias);
        __m128i val_hi_biased = _mm_xor_si128(val_hi, bias);
        __m128i vlow_biased = _mm_xor_si128(vlow, bias);
        __m128i vhigh_biased = _mm_xor_si128(vhigh, bias);
        
        // val >= low: signed compare (val_biased >= low_biased) => !(val_biased < low_biased)
        // val <= high: signed compare (val_biased <= high_biased) => !(val_biased > high_biased)
        
        // cmplt gives -1 where val < low
        __m128i lt_low_lo = _mm_cmplt_epi32(val_lo_biased, vlow_biased);
        __m128i lt_low_hi = _mm_cmplt_epi32(val_hi_biased, vlow_biased);
        
        // cmpgt gives -1 where val > high
        __m128i gt_high_lo = _mm_cmpgt_epi32(val_lo_biased, vhigh_biased);
        __m128i gt_high_hi = _mm_cmpgt_epi32(val_hi_biased, vhigh_biased);
        
        // in_range = !(lt_low | gt_high)
        __m128i out_of_range_lo = _mm_or_si128(lt_low_lo, gt_high_lo);
        __m128i out_of_range_hi = _mm_or_si128(lt_low_hi, gt_high_hi);
        
        __m128i mask_lo = _mm_andnot_si128(out_of_range_lo, _mm_set1_epi32(-1));
        __m128i mask_hi = _mm_andnot_si128(out_of_range_hi, _mm_set1_epi32(-1));
        
        // Mask the values
        __m128i masked_lo = _mm_and_si128(val_lo, mask_lo);
        __m128i masked_hi = _mm_and_si128(val_hi, mask_hi);
        
        // Accumulate as 64-bit to avoid overflow
        // Unpack to 64-bit and add
        __m128i zero = _mm_setzero_si128();
        __m128i sum_lo_0 = _mm_unpacklo_epi32(masked_lo, zero);
        __m128i sum_lo_1 = _mm_unpackhi_epi32(masked_lo, zero);
        __m128i sum_hi_0 = _mm_unpacklo_epi32(masked_hi, zero);
        __m128i sum_hi_1 = _mm_unpackhi_epi32(masked_hi, zero);
        
        vsum_lo = _mm_add_epi64(vsum_lo, sum_lo_0);
        vsum_lo = _mm_add_epi64(vsum_lo, sum_lo_1);
        vsum_hi = _mm_add_epi64(vsum_hi, sum_hi_0);
        vsum_hi = _mm_add_epi64(vsum_hi, sum_hi_1);
      }
      
      // Horizontal sum
      __m128i vsum = _mm_add_epi64(vsum_lo, vsum_hi);
      uint64_t tmp[2];
      _mm_storeu_si128(reinterpret_cast<__m128i*>(tmp), vsum);
      sum = tmp[0] + tmp[1];
    }
#endif
    
    // Scalar tail
    for (; i < n; ++i) {
      uint32_t val = base + static_cast<uint32_t>(data[i]);
      if (val >= low && val <= high) {
        sum += val;
      }
    }
  }
  
  return sum;
}