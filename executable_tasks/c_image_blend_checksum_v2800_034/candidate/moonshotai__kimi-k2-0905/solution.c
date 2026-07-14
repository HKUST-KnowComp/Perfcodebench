#include "interface.h"

#include <stdint.h>
#include <immintrin.h>
#include <string.h>
#include <stdlib.h>

/* 8-way AVX2 blending */
static inline void blend8_rgba(const uint8_t* __restrict src,
                               const uint8_t* __restrict dst,
                               uint8_t* __restrict out)
{
    __m256i srcv = _mm256_loadu_si256((const __m256i*)src);
    __m256i dstv = _mm256_loadu_si256((const __m256i*)dst);
    __m256i ones = _mm256_set1_epi16(255);

    /* extract alpha channel (broadcast each alpha byte across its pixel) */
    __m256i zeros = _mm256_setzero_si256();
    __m256i as = _mm256_shuffle_epi8(srcv, _mm256_set_epi8(
        15,15,15,15,  15,15,15,15,  11,11,11,11,  11,11,11,11,
        7,7,7,7,  7,7,7,7,  3,3,3,3,  3,3,3,3));

    /* process lanes in 16-bit */
    __m256i src_lo = _mm256_unpacklo_epi8(srcv, zeros);
    __m256i src_hi = _mm256_unpackhi_epi8(srcv, zeros);
    __m256i dst_lo = _mm256_unpacklo_epi8(dstv, zeros);
    __m256i dst_hi = _mm256_unpackhi_epi8(dstv, zeros);
    __m256i as_lo = _mm256_unpacklo_epi8(as, zeros);
    __m256i as_hi = _mm256_unpackhi_epi8(as, zeros);

    __m256i ainv_lo = _mm256_sub_epi16(ones, as_lo);
    __m256i ainv_hi = _mm256_sub_epi16(ones, as_hi);

    __m256i rc_lo = _mm256_add_epi16(_mm256_mullo_epi16(src_lo, as_lo),
                                     _mm256_mullo_epi16(dst_lo, ainv_lo));
    rc_lo = _mm256_add_epi16(rc_lo, _mm256_set1_epi16(127));
    rc_lo = _mm256_srli_epi16(rc_lo, 8);

    __m256i rc_hi = _mm256_add_epi16(_mm256_mullo_epi16(src_hi, as_hi),
                                     _mm256_mullo_epi16(dst_hi, ainv_hi));
    rc_hi = _mm256_add_epi16(rc_hi, _mm256_set1_epi16(127));
    rc_hi = _mm256_srli_epi16(rc_hi, 8);

    __m256i res_lo = _mm256_packus_epi16(rc_lo, rc_hi);
    __m256i res_32 = _mm256_permute4x64_epi64(res_lo, 0xD8);

    /* force alpha to 255 */
    __m256i alpha_mask = _mm256_set1_epi32(0xFF000000);
    res_32 = _mm256_or_si256(res_32, alpha_mask);

    _mm256_storeu_si256((__m256i*)out, res_32);
}

static uint64_t checksum_u8(const uint8_t* data, size_t n)
{
    uint64_t h = 14695981039346656037ULL;
    const uint64_t prime = 1099511628211ULL;
    for (size_t i = 0; i < n; ++i) {
        h ^= (uint64_t)data[i];
        h *= prime;
    }
    return h;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters)
{
    size_t pixels = (size_t)width * (size_t)height;
    size_t bytes = pixels * 4;
    uint8_t* out = (uint8_t*)malloc(bytes);
    if (out == NULL) return 0;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        /* process blocks of 8 pixels */
        size_t i = 0;
        for (; i + 8 <= pixels; i += 8) {
            blend8_rgba(src + i*4, dst + i*4, out + i*4);
        }
        /* scalar tail (<8 pixels) */
        for (; i < pixels; ++i) {
            const size_t idx = 4*i;
            const uint8_t a = src[idx+3];
            uint32_t s0 = (uint32_t)src[idx+0];
            uint32_t d0 = (uint32_t)dst[idx+0];
            uint32_t s1 = (uint32_t)src[idx+1];
            uint32_t d1 = (uint32_t)dst[idx+1];
            uint32_t s2 = (uint32_t)src[idx+2];
            uint32_t d2 = (uint32_t)dst[idx+2];

            out[idx]   = (uint8_t)((s0*a + d0*(255-a) + 127)/255);
            out[idx+1] = (uint8_t)((s1*a + d1*(255-a) + 127)/255);
            out[idx+2] = (uint8_t)((s2*a + d2*(255-a) + 127)/255);
            out[idx+3] = 255;
        }
        hash = checksum_u8(out, bytes);
    }
    free(out);
    return hash;
}