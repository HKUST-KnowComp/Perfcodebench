#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < pixels; ++i) {
            size_t idx = i * 4;
            uint8_t a = src[idx + 3];
            uint32_t inv_a = 255u - a;

            uint32_t r_num = (uint32_t)src[idx] * a + (uint32_t)dst[idx] * inv_a + 127u;
            uint32_t g_num = (uint32_t)src[idx+1] * a + (uint32_t)dst[idx+1] * inv_a + 127u;
            uint32_t b_num = (uint32_t)src[idx+2] * a + (uint32_t)dst[idx+2] * inv_a + 127u;

            uint8_t r = r_num / 255u;
            uint8_t g = g_num / 255u;
            uint8_t b = b_num / 255u;

            hash ^= r; hash *= FNV_PRIME;
            hash ^= g; hash *= FNV_PRIME;
            hash ^= b; hash *= FNV_PRIME;
            hash ^= 255u; hash *= FNV_PRIME;
        }
    }
    return hash;
}
