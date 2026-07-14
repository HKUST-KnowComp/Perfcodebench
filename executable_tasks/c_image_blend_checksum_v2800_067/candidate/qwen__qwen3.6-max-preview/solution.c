#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    size_t n_bytes = pixels * 4;
    uint64_t hash = 0;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME  = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < n_bytes; i += 4) {
            uint8_t a = src[i + 3];
            uint8_t inv_a = 255u - a;

            uint32_t r_num = (uint32_t)src[i] * a + (uint32_t)dst[i] * inv_a + 127u;
            uint8_t r = (uint8_t)((r_num * 257u + 32895u) >> 16);

            uint32_t g_num = (uint32_t)src[i + 1] * a + (uint32_t)dst[i + 1] * inv_a + 127u;
            uint8_t g = (uint8_t)((g_num * 257u + 32895u) >> 16);

            uint32_t b_num = (uint32_t)src[i + 2] * a + (uint32_t)dst[i + 2] * inv_a + 127u;
            uint8_t b = (uint8_t)((b_num * 257u + 32895u) >> 16);

            hash ^= r; hash *= FNV_PRIME;
            hash ^= g; hash *= FNV_PRIME;
            hash ^= b; hash *= FNV_PRIME;
            hash ^= 255u; hash *= FNV_PRIME;
        }
    }
    return hash;
}