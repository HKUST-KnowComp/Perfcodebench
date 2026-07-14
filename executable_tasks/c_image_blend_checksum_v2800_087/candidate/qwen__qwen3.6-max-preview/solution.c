#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    if (iters <= 0) return 0;
    size_t pixels = (size_t)width * (size_t)height;
    size_t nbytes = pixels * 4u;
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < nbytes; i += 4u) {
            uint32_t a = src[i + 3u];
            uint32_t inv_a = 255u - a;

            uint32_t b0 = (src[i] * a + dst[i] * inv_a + 127u) / 255u;
            uint32_t b1 = (src[i + 1u] * a + dst[i + 1u] * inv_a + 127u) / 255u;
            uint32_t b2 = (src[i + 2u] * a + dst[i + 2u] * inv_a + 127u) / 255u;

            hash ^= b0; hash *= 1099511628211ULL;
            hash ^= b1; hash *= 1099511628211ULL;
            hash ^= b2; hash *= 1099511628211ULL;
            hash ^= 255u; hash *= 1099511628211ULL;
        }
    }
    return hash;
}