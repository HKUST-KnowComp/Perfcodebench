#include "interface.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>

// Optimized FNV-1a constants
#define FNV_OFFSET_BASIS 1469598103934665603ULL
#define FNV_PRIME 1099511628211ULL

static inline uint64_t fnv1a_update(uint64_t hash, const uint8_t* data, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= FNV_PRIME;
    }
    return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t total_bytes = pixels * 4;
    uint8_t* out = (uint8_t*)aligned_alloc(32, total_bytes);
    if (!out) return 0;

    uint64_t hash = 0;
    const __m128i v255 = _mm_set1_epi16(255);
    const __m128i v127 = _mm_set1_epi16(127);
    const __m128i vAlphaMask = _mm_set1_epi32(0xFF000000);

    for (int iter = 0; iter < iters; ++iter) {
        size_t i = 0;
        // Process 4 pixels at a time using SSE
        for (; i + 3 < pixels; i += 4) {
            __m128i s = _mm_loadu_si128((const __m128i*)&src[i * 4]);
            __m128i d = _mm_loadu_si128((const __m128i*)&dst[i * 4]);

            // Unpack bytes to 16-bit words
            __m128i s_lo = _mm_unpacklo_epi8(s, _mm_setzero_si128());
            __m128i s_hi = _mm_unpackhi_epi8(s, _mm_setzero_si128());
            __m128i d_lo = _mm_unpacklo_epi8(d, _mm_setzero_si128());
            __m128i d_hi = _mm_unpackhi_epi8(d, _mm_setzero_si128());

            // Extract alpha and broadcast for each pixel
            // Alpha is at index 3, 7, 11, 15 in the original 16-byte vector
            __m128i a_lo = _mm_shuffle_epi8(s_lo, _mm_set_epi8(-1, 7, -1, 7, -1, 7, -1, 7, -1, 1, -1, 1, -1, 1, -1, 1));
            __m128i a_hi = _mm_shuffle_epi8(s_hi, _mm_set_epi8(-1, 7, -1, 7, -1, 7, -1, 7, -1, 1, -1, 1, -1, 1, -1, 1));

            // Blend: (s * a + d * (255 - a) + 127) / 255
            __m128i res_lo = _mm_add_epi16(_mm_mullo_epi16(s_lo, a_lo), _mm_mullo_epi16(d_lo, _mm_sub_epi16(v255, a_lo)));
            res_lo = _mm_add_epi16(res_lo, v127);
            // Fast division by 255: (x + 1 + (x >> 8)) >> 8
            res_lo = _mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(res_lo, _mm_set1_epi16(1)), _mm_srli_epi16(res_lo, 8)), 8);

            __m128i res_hi = _mm_add_epi16(_mm_mullo_epi16(s_hi, a_hi), _mm_mullo_epi16(d_hi, _mm_sub_epi16(v255, a_hi)));
            res_hi = _mm_add_epi16(res_hi, v127);
            res_hi = _mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(res_hi, _mm_set1_epi16(1)), _mm_srli_epi16(res_hi, 8)), 8);

            // Pack back to bytes and force alpha to 255
            __m128i res = _mm_packus_epi16(res_lo, res_hi);
            res = _mm_or_si128(res, vAlphaMask);
            _mm_storeu_si128((__m128i*)&out[i * 4], res);
        }

        // Scalar fallback for remaining pixels
        for (; i < pixels; ++i) {
            size_t idx = i * 4;
            uint32_t a = src[idx + 3];
            uint32_t inv_a = 255 - a;
            out[idx]     = (uint8_t)(((uint32_t)src[idx] * a + (uint32_t)dst[idx] * inv_a + 127) / 255);
            out[idx + 1] = (uint8_t)(((uint32_t)src[idx + 1] * a + (uint32_t)dst[idx + 1] * inv_a + 127) / 255);
            out[idx + 2] = (uint8_t)(((uint32_t)src[idx + 2] * a + (uint32_t)dst[idx + 2] * inv_a + 127) / 255);
            out[idx + 3] = 255;
        }

        hash = fnv1a_update(FNV_OFFSET_BASIS, out, total_bytes);
    }

    free(out);
    return hash;
}
