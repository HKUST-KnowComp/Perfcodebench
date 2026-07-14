#include "interface.h"

#include <cstdint>
#include <vector>

#if defined(__AVX2__)
#include <immintrin.h>
#define USE_AVX2 1
#elif defined(__SSE2__)
#include <emmintrin.h>
#define USE_SSE2 1
#endif

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    uint64_t sum = 0;
    const uint8_t* data = packed.data();
    const std::size_t n = packed.size();

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t i = 0;

#if USE_AVX2
        if (n >= 32) {
            __m256i mask_lo = _mm256_set1_epi8(0x0F);
            __m256i thresh = _mm256_set1_epi8(static_cast<char>(threshold));
            __m256i acc = _mm256_setzero_si256();
            
            for (; i + 31 < n; i += 32) {
                __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
                __m256i lo = _mm256_and_si256(v, mask_lo);
                __m256i hi = _mm256_and_si256(_mm256_srli_epi16(v, 4), mask_lo);
                
                // Compare > threshold: use saturating subtract, non-zero means > threshold
                __m256i lo_above = _mm256_subs_epu8(lo, thresh);
                __m256i hi_above = _mm256_subs_epu8(hi, thresh);
                
                // Create masks for values > threshold
                __m256i lo_mask = _mm256_cmpgt_epi8(lo_above, _mm256_setzero_si256());
                __m256i hi_mask = _mm256_cmpgt_epi8(hi_above, _mm256_setzero_si256());
                
                // Zero out values not above threshold
                lo = _mm256_and_si256(lo, lo_mask);
                hi = _mm256_and_si256(hi, hi_mask);
                
                // Sum using SAD against zero for horizontal sum of bytes
                __m256i sum_lo = _mm256_sad_epu8(lo, _mm256_setzero_si256());
                __m256i sum_hi = _mm256_sad_epu8(hi, _mm256_setzero_si256());
                
                acc = _mm256_add_epi64(acc, sum_lo);
                acc = _mm256_add_epi64(acc, sum_hi);
            }
            
            // Horizontal sum of acc
            __m128i acc_lo = _mm256_castsi256_si128(acc);
            __m128i acc_hi = _mm256_extracti128_si256(acc, 1);
            __m128i acc128 = _mm_add_epi64(acc_lo, acc_hi);
            sum = static_cast<uint64_t>(_mm_extract_epi64(acc128, 0)) + 
                  static_cast<uint64_t>(_mm_extract_epi64(acc128, 1));
        }
#elif USE_SSE2
        if (n >= 16) {
            __m128i mask_lo = _mm_set1_epi8(0x0F);
            __m128i thresh = _mm_set1_epi8(static_cast<char>(threshold));
            __m128i acc = _mm_setzero_si128();
            
            for (; i + 15 < n; i += 16) {
                __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
                __m128i lo = _mm_and_si128(v, mask_lo);
                __m128i hi = _mm_and_si128(_mm_srli_epi16(v, 4), mask_lo);
                
                // Compare > threshold
                __m128i lo_above = _mm_subs_epu8(lo, thresh);
                __m128i hi_above = _mm_subs_epu8(hi, thresh);
                
                __m128i zero = _mm_setzero_si128();
                __m128i lo_mask = _mm_cmpgt_epi8(lo_above, zero);
                __m128i hi_mask = _mm_cmpgt_epi8(hi_above, zero);
                
                lo = _mm_and_si128(lo, lo_mask);
                hi = _mm_and_si128(hi, hi_mask);
                
                __m128i sum_lo = _mm_sad_epu8(lo, zero);
                __m128i sum_hi = _mm_sad_epu8(hi, zero);
                
                acc = _mm_add_epi64(acc, sum_lo);
                acc = _mm_add_epi64(acc, sum_hi);
            }
            
            uint64_t tmp[2];
            _mm_storeu_si128(reinterpret_cast<__m128i*>(tmp), acc);
            sum = tmp[0] + tmp[1];
        }
#endif

        // Scalar remainder
        for (; i < n; ++i) {
            uint8_t byte = data[i];
            uint8_t lo = byte & 0x0Fu;
            uint8_t hi = byte >> 4u;
            if (lo > threshold) sum += lo;
            if (hi > threshold) sum += hi;
        }
    }
    return sum;
}