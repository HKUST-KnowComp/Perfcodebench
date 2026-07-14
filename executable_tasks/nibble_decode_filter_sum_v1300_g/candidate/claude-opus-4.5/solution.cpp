#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__SSE2__) && defined(__SSE4_1__)
#include <emmintrin.h>
#include <smmintrin.h>
#define USE_SSE 1
#else
#define USE_SSE 0
#endif

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const size_t n = packed.size();

#if USE_SSE
    const __m128i mask_lo = _mm_set1_epi8(0x0F);
    const __m128i thresh_vec = _mm_set1_epi8(static_cast<char>(threshold));
    
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t i = 0;
        
        __m128i acc0 = _mm_setzero_si128();
        __m128i acc1 = _mm_setzero_si128();
        __m128i acc2 = _mm_setzero_si128();
        __m128i acc3 = _mm_setzero_si128();
        int batch_count = 0;
        
        for (; i + 16 <= n; i += 16) {
            __m128i bytes = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
            
            __m128i lo = _mm_and_si128(bytes, mask_lo);
            __m128i hi = _mm_srli_epi16(bytes, 4);
            hi = _mm_and_si128(hi, mask_lo);
            
            __m128i cmp_lo = _mm_cmpgt_epi8(lo, thresh_vec);
            __m128i cmp_hi = _mm_cmpgt_epi8(hi, thresh_vec);
            
            __m128i masked_lo = _mm_and_si128(lo, cmp_lo);
            __m128i masked_hi = _mm_and_si128(hi, cmp_hi);
            
            __m128i sum16_lo = _mm_sad_epu8(masked_lo, _mm_setzero_si128());
            __m128i sum16_hi = _mm_sad_epu8(masked_hi, _mm_setzero_si128());
            
            acc0 = _mm_add_epi64(acc0, sum16_lo);
            acc1 = _mm_add_epi64(acc1, sum16_hi);
            
            batch_count++;
            if (batch_count == 255) {
                acc2 = _mm_add_epi64(acc2, acc0);
                acc3 = _mm_add_epi64(acc3, acc1);
                acc0 = _mm_setzero_si128();
                acc1 = _mm_setzero_si128();
                batch_count = 0;
            }
        }
        
        acc2 = _mm_add_epi64(acc2, acc0);
        acc3 = _mm_add_epi64(acc3, acc1);
        __m128i total = _mm_add_epi64(acc2, acc3);
        
        uint64_t tmp[2];
        _mm_storeu_si128(reinterpret_cast<__m128i*>(tmp), total);
        sum = tmp[0] + tmp[1];
        
        for (; i < n; ++i) {
            uint8_t b = data[i];
            uint8_t lo_nib = b & 0x0F;
            uint8_t hi_nib = b >> 4;
            if (lo_nib > threshold) sum += lo_nib;
            if (hi_nib > threshold) sum += hi_nib;
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
