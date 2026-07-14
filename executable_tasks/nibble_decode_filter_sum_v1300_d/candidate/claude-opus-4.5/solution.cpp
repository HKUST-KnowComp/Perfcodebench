#include "interface.h"

#include <cstdint>
#include <cstddef>

#if defined(__SSE2__) && defined(__SSE4_1__)
#include <emmintrin.h>
#include <smmintrin.h>
#define USE_SSE 1
#endif

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
    const uint8_t* data = packed.data();
    const size_t n = packed.size();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t i = 0;

#if USE_SSE
        if (n >= 16) {
            __m128i mask_lo = _mm_set1_epi8(0x0F);
            __m128i thresh = _mm_set1_epi8(static_cast<char>(threshold));
            __m128i acc0 = _mm_setzero_si128();
            __m128i acc1 = _mm_setzero_si128();
            __m128i acc2 = _mm_setzero_si128();
            __m128i acc3 = _mm_setzero_si128();
            int batch_count = 0;

            for (; i + 16 <= n; i += 16) {
                __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
                __m128i lo = _mm_and_si128(v, mask_lo);
                __m128i hi = _mm_and_si128(_mm_srli_epi16(v, 4), mask_lo);

                // Compare greater than threshold (unsigned via max trick)
                __m128i cmp_lo = _mm_cmpgt_epi8(_mm_max_epu8(lo, thresh), thresh);
                __m128i cmp_hi = _mm_cmpgt_epi8(_mm_max_epu8(hi, thresh), thresh);

                // Zero out values not above threshold
                lo = _mm_and_si128(lo, cmp_lo);
                hi = _mm_and_si128(hi, cmp_hi);

                // Horizontal sum using SAD against zero
                acc0 = _mm_add_epi64(acc0, _mm_sad_epu8(lo, _mm_setzero_si128()));
                acc1 = _mm_add_epi64(acc1, _mm_sad_epu8(hi, _mm_setzero_si128()));

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
            sum += static_cast<uint64_t>(_mm_extract_epi64(total, 0)) +
                   static_cast<uint64_t>(_mm_extract_epi64(total, 1));
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