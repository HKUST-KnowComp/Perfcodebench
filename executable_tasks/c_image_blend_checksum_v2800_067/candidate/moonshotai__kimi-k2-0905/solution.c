#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <emmintrin.h>

/* -------------------------------------------------------------------------- */
/*  Blend helpers                                                             */
/* -------------------------------------------------------------------------- */

static inline __m128i blend_epi32(__m128i s, __m128i d, __m128i a) {
    /* a0,a1,a2,a3 in low bytes of each 32-bit lane                      */
    __m128i sa = _mm_unpacklo_epi8(a, a);          /* 8-bit -> 16-bit    */
    __m128i sb = _mm_unpackhi_epi8(a, a);
    __m128i s16 = _mm_unpacklo_epi8(s, _mm_setzero_si128());
    __m128i d16 = _mm_unpacklo_epi8(d, _mm_setzero_si128());

    __m128i p1 = _mm_mullo_epi16(s16, sa);
    __m128i p2 = _mm_mullo_epi16(d16, _mm_sub_epi16(_mm_set1_epi16(255), sa));
    __m128i sum = _mm_add_epi16(p1, p2);
    __m128i res = _mm_srli_epi16(_mm_add_epi16(sum, _mm_set1_epi16(127)), 8);
    return _mm_packus_epi16(res, res);
}

/* -------------------------------------------------------------------------- */
/*  Checksum                                                                  */
/* -------------------------------------------------------------------------- */

static inline uint64_t checksum_block(const uint8_t *data) {
    /* 64-byte block, 16×4 = 64 bytes                                           */
    const uint64_t prime = 1099511628211ULL;
    uint64_t h = 1469598103934665603ULL;
    for (int i = 0; i < 64; ++i) {
        h ^= data[i];
        h *= prime;
    }
    return h;
}

static uint64_t checksum_bytes(const uint8_t *data, size_t n) {
    uint64_t h = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    size_t i = 0;

    /* Process 64-byte blocks ------------------------------------------------- */
    for (; i + 64 <= n; i += 64) {
        h ^= checksum_block(data + i);
        h *= prime;
    }

    /* Tail -------------------------------------------------------------------- */
    for (; i < n; ++i) {
        h ^= data[i];
        h *= prime;
    }
    return h;
}

/* -------------------------------------------------------------------------- */
/*  Main kernel                                                               */
/* -------------------------------------------------------------------------- */

uint64_t run(const uint8_t *src, const uint8_t *dst, int width, int height, int iters) {
    const size_t pixels = (size_t)width * (size_t)height;
    uint8_t *out = (uint8_t *)malloc(pixels * 4u);
    if (!out) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0;

        /* SIMD: 4 pixels (16 bytes) per iteration ------------------------------ */
        for (; i + 4 <= pixels; i += 4) {
            size_t idx = i * 4u;
            __m128i s = _mm_loadu_si128((__m128i *)(src + idx));
            __m128i d = _mm_loadu_si128((__m128i *)(dst + idx));
            __m128i a = _mm_shuffle_epi8(s, _mm_set_epi8(
                -1, 3, -1, 3, -1, 3, -1, 3, -1, 3, -1, 3, -1, 3, -1, 3));

            __m128i rb = blend_epi32(s, d, a); /* red & blue blended            */

            /* green: same mask shifted                                           */
            __m128i g = _mm_shuffle_epi8(s, _mm_set_epi8(
                -1, 7, -1, 7, -1, 7, -1, 7, -1, 7, -1, 7, -1, 7, -1, 7));
            __m128i gblended = blend_epi32(_mm_srli_si128(s, 1),
                                           _mm_srli_si128(d, 1), g);
            __m128i g8 = _mm_slli_si128(gblended, 1);

            __m128i final = _mm_or_si128(_mm_and_si128(rb, _mm_set1_epi32(0x00FF00FF)),
                                         _mm_and_si128(g8,  _mm_set1_epi32(0x0000FF00)));
            final = _mm_or_si128(final, _mm_set1_epi32(0xFF000000));

            _mm_storeu_si128((__m128i *)(out + idx), final);
        }

        /* Scalar tail ------------------------------------------------------------ */
        for (; i < pixels; ++i) {
            size_t idx = i * 4u;
            uint8_t a = src[idx + 3];
            out[idx + 0] = (uint8_t)(((uint32_t)src[idx + 0] * a +
                                      (uint32_t)dst[idx + 0] * (255u - a) + 127u) / 255u);
            out[idx + 1] = (uint8_t)(((uint32_t)src[idx + 1] * a +
                                      (uint32_t)dst[idx + 1] * (255u - a) + 127u) / 255u);
            out[idx + 2] = (uint8_t)(((uint32_t)src[idx + 2] * a +
                                      (uint32_t)dst[idx + 2] * (255u - a) + 127u) / 255u);
            out[idx + 3] = 255u;
        }

        hash = checksum_bytes(out, pixels * 4u);
    }

    free(out);
    return hash;
}