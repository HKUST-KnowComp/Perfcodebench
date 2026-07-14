#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <emmintrin.h>

static inline __m128i blend_one(__m128i s, __m128i d, __m128i a)
{
    /* replicate alpha to all channels except alpha */
    __m128i alpha = _mm_shuffle_epi32(a, _MM_SHUFFLE(3,3,3,3));   /* a a a a */
    __m128i inv   = _mm_sub_epi16(_mm_set1_epi16(255), alpha);    /* 255-a */

    /* promote to 16-bit */
    __m128i s_lo = _mm_unpacklo_epi8(s, _mm_setzero_si128());
    __m128i s_hi = _mm_unpackhi_epi8(s, _mm_setzero_si128());
    __m128i d_lo = _mm_unpacklo_epi8(d, _mm_setzero_si128());
    __m128i d_hi = _mm_unpackhi_epi8(d, _mm_setzero_si128());

    /* (s*alpha + d*(255-alpha) + 127) / 255  -- 16-bit math */
    __m128i lo = _mm_mullo_epi16(s_lo, alpha);
    lo = _mm_add_epi16(lo, _mm_mullo_epi16(d_lo, inv));
    lo = _mm_add_epi16(lo, _mm_set1_epi16(127));
    lo = _mm_mulhi_epu16(lo, _mm_set1_epi16(0x8081)); /* (x*0x8081)>>16 == x/255 */

    __m128i hi = _mm_mullo_epi16(s_hi, alpha);
    hi = _mm_add_epi16(hi, _mm_mullo_epi16(d_hi, inv));
    hi = _mm_add_epi16(hi, _mm_set1_epi16(127));
    hi = _mm_mulhi_epu16(hi, _mm_set1_epi16(0x8081));

    /* pack back to 8-bit */
    __m128i res = _mm_packus_epi16(lo, hi);
    /* force alpha = 255 */
    res = _mm_or_si128(res, _mm_set1_epi32(0xFF000000));
    return res;
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n)
{
    const uint64_t prime = 1099511628211ULL;
    const uint64_t seed  = 1469598103934665603ULL;
    uint64_t hash = seed;

    /* 8-byte-at-a-time hashing */
    const uint64_t* w = (const uint64_t*)data;
    size_t words = n / 8;
    for (size_t i = 0; i < words; ++i) {
        hash ^= w[i];
        hash *= prime;
    }
    /* tail */
    for (size_t i = words * 8; i < n; ++i) {
        hash ^= data[i];
        hash *= prime;
    }
    return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters)
{
    size_t pixels = (size_t)width * (size_t)height;
    size_t bytes  = pixels * 4;

    uint8_t* out = (uint8_t*)aligned_alloc(16, bytes);
    if (!out) return 0;

    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* s = src;
        const uint8_t* d = dst;
        uint8_t* o = out;
        size_t i = 0;

        /* 4 pixels at a time */
        for (; i + 3 < pixels; i += 4) {
            __m128i src4 = _mm_loadu_si128((__m128i*)(s + i * 4));
            __m128i dst4 = _mm_loadu_si128((__m128i*)(d + i * 4));
            __m128i alpha = _mm_srli_epi32(src4, 24); /* extract alpha bytes */
            __m128i blended = blend_one(src4, dst4, alpha);
            _mm_storeu_si128((__m128i*)(o + i * 4), blended);
        }
        /* scalar tail */
        for (; i < pixels; ++i) {
            size_t idx = i * 4;
            uint8_t a = src[idx + 3];
            o[idx]     = (uint8_t)(((uint32_t)src[idx]     * a + (uint32_t)dst[idx]     * (255 - a) + 127) / 255);
            o[idx + 1] = (uint8_t)(((uint32_t)src[idx + 1] * a + (uint32_t)dst[idx + 1] * (255 - a) + 127) / 255);
            o[idx + 2] = (uint8_t)(((uint32_t)src[idx + 2] * a + (uint32_t)dst[idx + 2] * (255 - a) + 127) / 255);
            o[idx + 3] = 255;
        }

        hash = checksum_bytes(out, bytes);
    }
    free(out);
    return hash;
}