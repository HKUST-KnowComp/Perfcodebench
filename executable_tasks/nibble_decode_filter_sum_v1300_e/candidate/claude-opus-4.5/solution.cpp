#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__SSE4_1__) || defined(__x86_64__) || defined(_M_X64)
#include <emmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#define USE_SSE 1
#else
#define USE_SSE 0
#endif

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const size_t n = packed.size();
    const uint8_t* data = packed.data();

#if USE_SSE
    const __m128i mask_lo = _mm_set1_epi8(0x0F);
    const __m128i thresh_vec = _mm_set1_epi8(static_cast<char>(threshold));
    const __m128i ones = _mm_set1_epi8(1);
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        __m128i acc0 = _mm_setzero_si128();
        __m128i acc1 = _mm_setzero_si128();
        __m128i acc2 = _mm_setzero_si128();
        __m128i acc3 = _mm_setzero_si128();
        
        size_t i = 0;
        int batch_count = 0;
        
        for (; i + 16 <= n; i += 16) {
            __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
            
            __m128i lo = _mm_and_si128(chunk, mask_lo);
            __m128i hi = _mm_and_si128(_mm_srli_epi16(chunk, 4), mask_lo);
            
            // Compare > threshold using saturating subtract: if val > thresh, val - thresh - 1 >= 0 after saturation
            // Actually use: val > thresh iff (val - thresh - 1) doesn't underflow, i.e., _mm_subs_epu8(val, thresh+1) > 0
            // But simpler: use _mm_cmpgt_epi8 with signed comparison trick
            // Since nibbles are 0-15 and threshold is 0-15, we can use unsigned comparison
            // val > thresh iff _mm_subs_epu8(val, thresh) > 0 AND val != thresh
            // Easier: _mm_cmpgt_epu8 doesn't exist, but we can use: val > thresh iff _mm_max_epu8(val, thresh+1) == val AND thresh < 255
            
            // Use signed comparison by offsetting: treat as signed where 0-15 maps to -128 to -113
            // Or simpler: _mm_subs_epu8(val, thresh) gives val-thresh if val>=thresh, else 0
            // Then check if result > 0 AND original val > thresh
            // val > thresh iff _mm_subs_epu8(val, thresh) > 0 when val > thresh
            // Actually _mm_subs_epu8(val, thresh) = val - thresh if val >= thresh, else 0
            // So _mm_subs_epu8(val, thresh) > 0 iff val > thresh. Perfect!
            
            __m128i diff_lo = _mm_subs_epu8(lo, thresh_vec);
            __m128i diff_hi = _mm_subs_epu8(hi, thresh_vec);
            
            // Create masks: 0xFF where diff > 0, else 0
            __m128i zero = _mm_setzero_si128();
            __m128i mask_lo_cmp = _mm_cmpgt_epi8(diff_lo, zero); // signed cmp but diff is 0-15, so works
            __m128i mask_hi_cmp = _mm_cmpgt_epi8(diff_hi, zero);
            
            // Masked values
            __m128i masked_lo = _mm_and_si128(lo, mask_lo_cmp);
            __m128i masked_hi = _mm_and_si128(hi, mask_hi_cmp);
            
            // Use SAD against zero to horizontally sum bytes into 64-bit lanes
            __m128i sad_lo = _mm_sad_epu8(masked_lo, zero);
            __m128i sad_hi = _mm_sad_epu8(masked_hi, zero);
            
            acc0 = _mm_add_epi64(acc0, sad_lo);
            acc1 = _mm_add_epi64(acc1, sad_hi);
        }
        
        // Horizontal sum of accumulators
        __m128i acc_sum = _mm_add_epi64(acc0, acc1);
        acc_sum = _mm_add_epi64(acc_sum, _mm_srli_si128(acc_sum, 8));
        sum = static_cast<uint64_t>(_mm_cvtsi128_si64(acc_sum));
        
        // Scalar remainder
        for (; i < n; ++i) {
            uint8_t b = data[i];
            uint8_t lo = b & 0x0F;
            uint8_t hi = b >> 4;
            if (lo > threshold) sum += lo;
            if (hi > threshold) sum += hi;
        }
    }
#else
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < n; ++i) {
            uint8_t b = data[i];
            uint8_t lo = b & 0x0F;
            uint8_t hi = b >> 4;
            if (lo > threshold) sum += lo;
            if (hi > threshold) sum += hi;
        }
    }
#endif
    
    return sum;
}
