#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <emmintrin.h>   /* SSE2 */

static uint64_t checksum_bytes(const uint8_t* data, size_t n)
{
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters)
{
    const size_t pixels = (size_t)width * (size_t)height;
    const size_t bytes  = pixels * 4u;

    /* one persistent buffer */
    uint8_t* out = (uint8_t*)malloc(bytes);
    if (!out) return 0;

    /* constants kept in registers across pixels/iterations */
    const __m128i v255   = _mm_set1_epi16(255);
    const __m128i v127   = _mm_set1_epi16(127);
    const __m128i vzero  = _mm_setzero_si128();

    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0;

        /* process 4 pixels (16 bytes) per iteration */
        for (; i + 4 <= pixels; i += 4) {
            /* load 4 RGBA pixels from src & dst */
            __m128i s = _mm_loadu_si128((__m128i const*)(src + i * 4));
            __m128i d = _mm_loadu_si128((__m128i const*)(dst + i * 4));

            /* extract alpha bytes (src[3,7,11,15]) and splat each to 16-bit pairs */
            __m128i a16 = _mm_cvtepu8_epi16(_mm_srli_si128(s, 3));   /* 8 alphas duplicated */

            /* expand src & dst to 16-bit */
            __m128i s_lo = _mm_unpacklo_epi8(s, vzero);
            __m128i s_hi = _mm_unpackhi_epi8(s, vzero);
            __m128i d_lo = _mm_unpacklo_epi8(d, vzero);
            __m128i d_hi = _mm_unpackhi_epi8(d, vzero);

            /* (255 - alpha) */
            __m128i inv_lo = _mm_sub_epi16(v255, _mm_unpacklo_epi8(a16, vzero));
            __m128i inv_hi = _mm_sub_epi16(v255, _mm_unpackhi_epi8(a16, vzero));

            /* blend: (s*alpha + d*(255-alpha) + 127) / 255 */
            __m128i prod1_lo = _mm_mullo_epi16(s_lo, _mm_unpacklo_epi8(a16, vzero));
            __m128i prod2_lo = _mm_mullo_epi16(d_lo, inv_lo);
            __m128i sum_lo   = _mm_add_epi16(_mm_add_epi16(prod1_lo, prod2_lo), v127);
            __m128i res_lo   = _mm_srli_epi16(_mm_mulhi_epu16(sum_lo, _mm_set1_epi16(0x8081)), 7);

            __m128i prod1_hi = _mm_mullo_epi16(s_hi, _mm_unpackhi_epi8(a16, vzero));
            __m128i prod2_hi = _mm_mullo_epi16(d_hi, inv_hi);
            __m128i sum_hi   = _mm_add_epi16(_mm_add_epi16(prod1_hi, prod2_hi), v127);
            __m128i res_hi   = _mm_srli_epi16(_mm_mulhi_epu16(sum_hi, _mm_set1_epi16(0x8081)), 7);

            /* pack back to 8-bit */
            __m128i res = _mm_packus_epi16(res_lo, res_hi);

            /* set alpha channel to 0xFF */
            res = _mm_or_si128(_mm128i(_mm_and_si128(res, _mm_set1_epi32(0x00FFFFFF))),
                               _mm_set1_epi32(0xFF000000));

            _mm_storeu_si128((__m128i*)(out + i * 4), res);
        }

        /* tail: scalar fallback for <4 pixels */
        for (; i < pixels; ++i) {
            size_t idx = i * 4u;
            uint8_t a  = src[idx + 3];
            uint8_t s  = src[idx];
            uint8_t d  = dst[idx];
            out[idx]   = (uint8_t)(((uint32_t)s * a + (uint32_t)d * (255u - a) + 127u) / 255u);

            s = src[idx + 1];
            d = dst[idx + 1];
            out[idx + 1] = (uint8_t)(((uint32_t)s * a + (uint32_t)d * (255u - a) + 127u) / 255u);

            s = src[idx + 2];
            d = dst[idx + 2];
            out[idx + 2] = (uint8_t)(((uint32_t)s * a + (uint32_t)d * (255u - a) + 127u) / 255u);

            out[idx + 3] = 255u;
        }

        /* compute checksum once per iteration directly from the buffer */
        hash = checksum_bytes(out, bytes);
    }

    free(out);
    return hash;
}
