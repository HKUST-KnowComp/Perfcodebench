#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        const uint8_t* restrict s = src;
        const uint8_t* restrict d = dst;
        for (size_t i = 0; i < pixels; ++i) {
            uint8_t a = s[3];
            uint32_t inv_a = 255u - a;

            uint32_t r = ((uint32_t)s[0] * a + (uint32_t)d[0] * inv_a + 127u) / 255u;
            uint32_t g = ((uint32_t)s[1] * a + (uint32_t)d[1] * inv_a + 127u) / 255u;
            uint32_t b = ((uint32_t)s[2] * a + (uint32_t)d[2] * inv_a + 127u) / 255u;

            h ^= r; h *= 1099511628211ULL;
            h ^= g; h *= 1099511628211ULL;
            h ^= b; h *= 1099511628211ULL;
            h ^= 255u; h *= 1099511628211ULL;

            s += 4;
            d += 4;
        }
        hash = h;
    }
    return hash;
}
