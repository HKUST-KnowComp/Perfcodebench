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

static inline __m128i blend_epi16(__m128i s, __m128i d, __m128i a)
{
    const __m128i round = _mm_set1_epi16(127);
    const __m128i scale = _mm_set1_epi16(255);
    __m128i lo = _mm_mullo_epi16(s, a);
    __m128i hi = _mm_mullo_epi16(d, _mm_sub_epi16(scale, a));
    __m128i sum = _mm_add_epi16(_mm_add_epi16(lo, hi), round);
    return _mm_srli_epi16(_mm_mulhi_epu16(sum, _mm_set1_epi16(0x8081)), 7);
}

static inline __m128i blend_four_pixels(__m128i src, __m128i dst)
{
    /* src layout: R G B A R G B A ...  (16 bytes = 4 pixels) */
    __m128i a16 = _mm_shufflelo_epi16(src, _MM_SHUFFLE(3,3,3,3));
    a16 = _mm_shufflehi_epi16(a16, _MM_SHUFFLE(3,3,3,3));
    __m128i rb = blend_epi16(_mm_and_si128(src, _mm_set1_epi16(0x00FF)),
                             _mm_and_si128(dst, _mm_set1_epi16(0x00FF)), a16);
    __m128i ga = blend_epi16(_mm_srli_epi16(src, 8),
                             _mm_srli_epi16(dst, 8), a16);
    __m128i out = _mm_or_si128(rb, _mm_slli_epi16(ga, 8));
    /* set alpha to 0xFFFF (255) for all four pixels */
    out = _mm_or_si128(out, _mm_set1_epi32(0xFF000000));
    return out;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters)
{
    size_t pixels = (size_t)width * (size_t)height;
    uint8_t* out = (uint8_t*)malloc(pixels * 4);
    if (!out) return 0;

    for (int iter = 0; iter < iters; ++iter) {
        const __m128i* s128 = (const __m128i*)src;
        const __m128i* d128 = (const __m128i*)dst;
        __m128i* o128       = (__m128i*)out;
        size_t i = 0;
        size_t limit = pixels / 4;

        for (; i < limit; ++i) {
            __m128i srcv = _mm_loadu_si128(s128 + i);
            __m128i dstv = _mm_loadu_si128(d128 + i);
            __m128i res  = blend_four_pixels(srcv, dstv);
            _mm_storeu_si128(o128 + i, res);
        }

        /* tail (fewer than 4 pixels) */
        for (i *= 4; i < pixels; ++i) {
            size_t idx = i * 4;
            uint8_t a  = src[idx + 3];
            uint32_t s, d;
            s = src[idx + 0]; d = dst[idx + 0];
            out[idx + 0] = (uint8_t)((s * a + d * (255 - a) + 127) / 255);
            s = src[idx + 1]; d = dst[idx + 1];
            out[idx + 1] = (uint8_t)((s * a + d * (255 - a) + 127) / 255);
            s = src[idx + 2]; d = dst[idx + 2];
            out[idx + 2] = (uint8_t)((s * a + d * (255 - a) + 127) / 255);
            out[idx + 3] = 255;
        }
    }

    uint64_t hash = checksum_bytes(out, pixels * 4);
    free(out);
    return hash;
}