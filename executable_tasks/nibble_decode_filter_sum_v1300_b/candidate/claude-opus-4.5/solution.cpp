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
    const __m128i zero = _mm_setzero_si128();

    for (int iter = 0; iter < iters; ++iter) {
        __m128i acc0 = _mm_setzero_si128();
        __m128i acc1 = _mm_setzero_si128();
        __m128i acc2 = _mm_setzero_si128();
        __m128i acc3 = _mm_setzero_si128();

        size_t i = 0;
        const size_t vec_end = n & ~15ULL;

        for (; i < vec_end; i += 16) {
            __m128i bytes = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
            __m128i lo = _mm_and_si128(bytes, mask_lo);
            __m128i hi = _mm_and_si128(_mm_srli_epi16(bytes, 4), mask_lo);

            // Compare greater than threshold (unsigned via saturating sub)
            __m128i lo_above = _mm_cmpgt_epi8(_mm_sub_epi8(lo, thresh_vec), zero);
            __m128i hi_above = _mm_cmpgt_epi8(_mm_sub_epi8(hi, thresh_vec), zero);

            // Mask values not above threshold
            lo = _mm_and_si128(lo, lo_above);
            hi = _mm_and_si128(hi, hi_above);

            // Unpack to 16-bit and accumulate
            __m128i lo_lo16 = _mm_unpacklo_epi8(lo, zero);
            __m128i lo_hi16 = _mm_unpackhi_epi8(lo, zero);
            __m128i hi_lo16 = _mm_unpacklo_epi8(hi, zero);
            __m128i hi_hi16 = _mm_unpackhi_epi8(hi, zero);

            acc0 = _mm_add_epi16(acc0, lo_lo16);
            acc1 = _mm_add_epi16(acc1, lo_hi16);
            acc2 = _mm_add_epi16(acc2, hi_lo16);
            acc3 = _mm_add_epi16(acc3, hi_hi16);

            // Prevent overflow: reduce every 4096 iterations (16-bit can hold 65535, max nibble=15)
            if ((i & 0xFFF0) == 0xFFF0 && i > 0) {
                // Widen to 32-bit and reduce
                __m128i sum32_0 = _mm_add_epi32(_mm_unpacklo_epi16(acc0, zero), _mm_unpackhi_epi16(acc0, zero));
                __m128i sum32_1 = _mm_add_epi32(_mm_unpacklo_epi16(acc1, zero), _mm_unpackhi_epi16(acc1, zero));
                __m128i sum32_2 = _mm_add_epi32(_mm_unpacklo_epi16(acc2, zero), _mm_unpackhi_epi16(acc2, zero));
                __m128i sum32_3 = _mm_add_epi32(_mm_unpacklo_epi16(acc3, zero), _mm_unpackhi_epi16(acc3, zero));
                __m128i total32 = _mm_add_epi32(_mm_add_epi32(sum32_0, sum32_1), _mm_add_epi32(sum32_2, sum32_3));
                // Horizontal sum
                total32 = _mm_add_epi32(total32, _mm_srli_si128(total32, 8));
                total32 = _mm_add_epi32(total32, _mm_srli_si128(total32, 4));
                sum += static_cast<uint32_t>(_mm_cvtsi128_si32(total32));
                acc0 = acc1 = acc2 = acc3 = zero;
            }
        }

        // Final reduction of accumulators
        __m128i sum32_0 = _mm_add_epi32(_mm_unpacklo_epi16(acc0, zero), _mm_unpackhi_epi16(acc0, zero));
        __m128i sum32_1 = _mm_add_epi32(_mm_unpacklo_epi16(acc1, zero), _mm_unpackhi_epi16(acc1, zero));
        __m128i sum32_2 = _mm_add_epi32(_mm_unpacklo_epi16(acc2, zero), _mm_unpackhi_epi16(acc2, zero));
        __m128i sum32_3 = _mm_add_epi32(_mm_unpacklo_epi16(acc3, zero), _mm_unpackhi_epi16(acc3, zero));
        __m128i total32 = _mm_add_epi32(_mm_add_epi32(sum32_0, sum32_1), _mm_add_epi32(sum32_2, sum32_3));
        total32 = _mm_add_epi32(total32, _mm_srli_si128(total32, 8));
        total32 = _mm_add_epi32(total32, _mm_srli_si128(total32, 4));
        sum += static_cast<uint32_t>(_mm_cvtsi128_si32(total32));

        // Scalar tail
        for (; i < n; ++i) {
            uint8_t lo = data[i] & 0x0F;
            uint8_t hi = data[i] >> 4;
            if (lo > threshold) sum += lo;
            if (hi > threshold) sum += hi;
        }

        // Reset sum for next iteration except last
        if (iter < iters - 1) sum = 0;
    }
#else
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < n; ++i) {
            uint8_t lo = data[i] & 0x0F;
            uint8_t hi = data[i] >> 4;
            if (lo > threshold) sum += lo;
            if (hi > threshold) sum += hi;
        }
    }
#endif

    return sum;
}