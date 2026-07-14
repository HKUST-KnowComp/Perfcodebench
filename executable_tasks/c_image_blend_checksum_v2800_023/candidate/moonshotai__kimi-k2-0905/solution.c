#include "interface.h"
#include <immintrin.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint64_t checksum_u64(const uint8_t *data, size_t n) {
    const uint64_t m = 0xC6A4A7935BD1E995ULL;
    const uint64_t seed = 0x1234567890ABCDEFULL;
    uint64_t h = seed ^ (n * m);
    size_t left = n;
    const uint8_t *p = data;
    while (left >= 8) {
        uint64_t k;
        memcpy(&k, p, 8);
        k *= m;
        k ^= k >> 47;
        h ^= k;
        h *= m;
        p += 8;
        left -= 8;
    }
    if (left) {
        uint64_t tail = 0;
        memcpy(&tail, p, left);
        tail *= m;
        tail ^= tail >> 47;
        h ^= tail;
        h *= m;
    }
    h ^= h >> 47;
    h *= m;
    h ^= h >> 47;
    return h;
}

uint64_t run(const uint8_t *src, const uint8_t *dst, int width, int height, int iters) {
    const size_t pixels = (size_t)width * (size_t)height;
    const size_t bytes = pixels * 4;
    uint8_t *out = (uint8_t *)malloc(bytes);
    if (!out) return 0;
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; iter++) {
        const uint8_t *s = src;
        const uint8_t *d = dst;
        uint8_t *o = out;
        size_t i = 0;
        const size_t simd_limit = pixels & ~(size_t)3;
        while (i < simd_limit) {
            __m128i s4 = _mm_loadu_si128((const __m128i *)(s));
            __m128i d4 = _mm_loadu_si128((const __m128i *)(d));
            __m128i a4 = _mm_shuffle_epi8(
                s4,
                _mm_set_epi8(-1, 3, 3, 3, -1, 7, 7, 7, -1, 11, 11, 11, -1, 15, 15, 15)
            );
            __m128i t1 = _mm_mullo_epi16(_mm_cvtepu8_epi16(s4), _mm_cvtepu8_epi16(a4));
            __m128i t2 = _mm_mullo_epi16(
                _mm_cvtepu8_epi16(d4),
                _mm_cvtepu8_epi16(_mm_sub_epi8(_mm_set1_epi16(255), a4))
            );
            __m128i sum = _mm_add_epi16(_mm_add_epi16(t1, t2), _mm_set1_epi16(127));
            __m128i hi = _mm_srli_epi16(sum, 8);
            __m128i lo = _mm_and_si128(sum, _mm_set1_epi16(0xFF));
            __m128i blended = _mm_or_si128(hi, lo);
            blended = _mm_packus_epi16(blended, blended);
            __m128i alpha_o = _mm_srli_si128(_mm_shuffle_epi8(
                _mm_set1_epi32(0xFF000000),
                _mm_set_epi8(12, 12, 12, 15, 8, 8, 8, 11, 4, 4, 4, 7, 0, 0, 0, 3)
            ), 12);
            __m128i res = _mm_or_si128(blended, alpha_o);
            _mm_storeu_si128((__m128i *)o, res);
            s += 16; d += 16; o += 16;
            i += 4;
        }
        while (i < pixels) {
            size_t idx = i * 4;
            uint8_t a = s[3];
            o[0] = (uint8_t)(((uint32_t)s[0] * (uint32_t)a + (uint32_t)d[0] * (uint32_t)(255 - a) + 127u) / 255u);
            o[1] = (uint8_t)(((uint32_t)s[1] * (uint32_t)a + (uint32_t)d[1] * (uint32_t)(255 - a) + 127u) / 255u);
            o[2] = (uint8_t)(((uint32_t)s[2] * (uint32_t)a + (uint32_t)d[2] * (uint32_t)(255 - a) + 127u) / 255u);
            o[3] = 255;
            s += 4; d += 4; o += 4;
            i++;
        }
        hash = checksum_u64(out, bytes);
    }
    free(out);
    return hash;
}