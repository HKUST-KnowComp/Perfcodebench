#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t total_pixels = (size_t)width * (size_t)height;
    const uint64_t HASH_INIT = 1469598103934665603ULL;
    const uint64_t HASH_MUL = 1099511628211ULL;
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = HASH_INIT;
        for (size_t i = 0; i < total_pixels; ++i) {
            size_t idx = i * 4u;
            uint8_t a = src[idx + 3u];
            uint8_t inv_a = 255u - a;
            uint8_t r = (uint8_t)(((uint32_t)src[idx]     * a + (uint32_t)dst[idx]     * inv_a + 127u) / 255u);
            uint8_t g = (uint8_t)(((uint32_t)src[idx + 1u] * a + (uint32_t)dst[idx + 1u] * inv_a + 127u) / 255u);
            uint8_t b = (uint8_t)(((uint32_t)src[idx + 2u] * a + (uint32_t)dst[idx + 2u] * inv_a + 127u) / 255u);
            uint8_t alpha = 255u;

            hash ^= (uint64_t)r;
            hash *= HASH_MUL;
            hash ^= (uint64_t)g;
            hash *= HASH_MUL;
            hash ^= (uint64_t)b;
            hash *= HASH_MUL;
            hash ^= (uint64_t)alpha;
            hash *= HASH_MUL;
        }
    }
    return hash;
}