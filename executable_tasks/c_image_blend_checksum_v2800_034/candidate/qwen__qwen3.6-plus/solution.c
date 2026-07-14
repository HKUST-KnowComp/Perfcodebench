#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = FNV_OFFSET;
        const uint8_t* s = src;
        const uint8_t* d = dst;
        for (size_t i = 0; i < pixels; ++i) {
            uint8_t a = s[3];
            uint8_t inv_a = 255 - a;

            uint32_t r = ((uint32_t)s[0] * a + (uint32_t)d[0] * inv_a + 127) / 255;
            h ^= r; h *= FNV_PRIME;

            uint32_t g = ((uint32_t)s[1] * a + (uint32_t)d[1] * inv_a + 127) / 255;
            h ^= g; h *= FNV_PRIME;

            uint32_t b = ((uint32_t)s[2] * a + (uint32_t)d[2] * inv_a + 127) / 255;
            h ^= b; h *= FNV_PRIME;

            h ^= 255; h *= FNV_PRIME;

            s += 4;
            d += 4;
        }
        hash = h;
    }
    return hash;
}