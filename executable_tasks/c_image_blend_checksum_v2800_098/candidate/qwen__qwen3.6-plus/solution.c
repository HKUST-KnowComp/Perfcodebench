#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    (void)width;
    (void)height;
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        const uint8_t* s = src;
        const uint8_t* d = dst;
        for (size_t i = 0; i < pixels; ++i) {
            uint32_t a = s[3];
            uint32_t ia = 255 - a;

            uint8_t r = (uint8_t)(((uint32_t)s[0] * a + (uint32_t)d[0] * ia + 127) / 255);
            uint8_t g = (uint8_t)(((uint32_t)s[1] * a + (uint32_t)d[1] * ia + 127) / 255);
            uint8_t b = (uint8_t)(((uint32_t)s[2] * a + (uint32_t)d[2] * ia + 127) / 255);

            s += 4;
            d += 4;

            hash ^= r; hash *= FNV_PRIME;
            hash ^= g; hash *= FNV_PRIME;
            hash ^= b; hash *= FNV_PRIME;
            hash ^= 255; hash *= FNV_PRIME;
        }
    }
    return hash;
}
