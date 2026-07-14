#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <emmintrin.h>

static inline __m128i blend_rgba(__m128i s, __m128i d)
{
    /*
     * Layout after _mm_unpacklo/hi_epi8 (low 64 bits shown):
     * s:  b7 g7 r7 a7 ... b0 g0 r0 a0   (16 bytes)
     * d:  same layout for destination
     *
     * Goal: compute blended RGB, keep dest alpha 255.
     */
    __m128i zero = _mm_setzero_si128();
    __m128i s_lo = _mm_unpacklo_epi8(s, zero);
    __m128i s_hi = _mm_unpackhi_epi8(s, zero);
    __m128i d_lo = _mm_unpacklo_epi8(d, zero);
    __m128i d_hi = _mm_unpackhi_epi8(d, zero);

    /* Extract alpha for each pixel and replicate across RGBA lanes.
       Alpha is in bytes 3,7,11,15. */
    __m128i alpha = _mm_shuffle_epi32(_mm_shuffle_epi8(s,
                                   _mm_setr_epi8(3,3,3,3, 7,7,7,7, 11,11,11,11, 15,15,15,15)),
                                   _MM_SHUFFLE(3,2,1,0));
    __m128i a_lo = _mm_unpacklo_epi8(alpha, zero);
    __m128i a_hi = _mm_unpackhi_epi8(alpha, zero);

    /* d * (255 - a) */
    __m128i da_lo = _mm_sub_epi16(_mm_set1_epi16(255), a_lo);
    __m128i da_hi = _mm_sub_epi16(_mm_set1_epi16(255), a_hi);
    da_lo = _mm_mullo_epi16(d_lo, da_lo);
    da_hi = _mm_mullo_epi16(d_hi, da_hi);

    /* s * a */
    __m128i sa_lo = _mm_mullo_epi16(s_lo, a_lo);
    __m128i sa_hi = _mm_mullo_epi16(s_hi, a_hi);

    /* Sum + 127 for rounding */
    __m128i sum_lo = _mm_add_epi16(sa_lo, da_lo);
    __m128i sum_hi = _mm_add_epi16(sa_hi, da_hi);
    sum_lo = _mm_add_epi16(sum_lo, _mm_set1_epi16(127));
    sum_hi = _mm_add_epi16(sum_hi, _mm_set1_epi16(127));

    /* Divide by 255 using multiply-high trick: x/255 ≈ (x*0x8081)>>23 */
    const __m128i mul = _mm_set1_epi16(0x8081);
    sum_lo = _mm_mulhi_epu16(sum_lo, mul);
    sum_hi = _mm_mulhi_epu16(sum_hi, mul);

    /* Pack back to RGBA, force alpha=255 */
    __m128i out = _mm_packus_epi16(sum_lo, sum_hi);
    out = _mm_or_si128(out, _mm_set1_epi32(0xFF000000));
    return out;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters)
{
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* out = (uint8_t*)malloc(pixels * 4u);
        if (!out) return 0;

        const __m128i* s = (const __m128i*)src;
        const __m128i* d = (const __m128i*)dst;
        __m128i* o = (__m128i*)out;
        size_t i = 0;

        /* Blend 4 pixels (16 bytes) per iteration */
        for (; i + 15 < pixels * 4; i += 16) {
            __m128i sv = _mm_loadu_si128(s++);
            __m128i dv = _mm_loadu_si128(d++);
            __m128i res = blend_rgba(sv, dv);
            _mm_storeu_si128(o++, res);
        }

        /* Tail: scalar fallback for remaining <4 pixels */
        for (; i < pixels * 4; i += 4) {
            uint8_t a = src[i + 3];
            out[i + 0] = (uint8_t)(((uint32_t)src[i + 0] * a + (uint32_t)dst[i + 0] * (255u - a) + 127u) / 255u);
            out[i + 1] = (uint8_t)(((uint32_t)src[i + 1] * a + (uint32_t)dst[i + 1] * (255u - a) + 127u) / 255u);
            out[i + 2] = (uint8_t)(((uint32_t)src[i + 2] * a + (uint32_t)dst[i + 2] * (255u - a) + 127u) / 255u);
            out[i + 3] = 255u;
        }

        /* Fused checksum while data is hot in cache */
        uint64_t h = 1469598103934665603ULL;
        const uint8_t* p = out;
        for (size_t j = 0; j < pixels * 4; ++j) {
            h ^= (uint64_t)p[j];
            h *= 1099511628211ULL;
        }
        hash = h;
        free(out);
    }
    return hash;
}