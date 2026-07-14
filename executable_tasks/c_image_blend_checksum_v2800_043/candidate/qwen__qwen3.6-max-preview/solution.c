#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    (void)iters;
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    const uint8_t* s = src;
    const uint8_t* d = dst;

    for (size_t i = 0; i < pixels; ++i) {
        uint32_t a = s[3];
        uint32_t inv_a = 255u - a;

        uint32_t v0 = (uint32_t)s[0] * a + (uint32_t)d[0] * inv_a;
        uint32_t v1 = (uint32_t)s[1] * a + (uint32_t)d[1] * inv_a;
        uint32_t v2 = (uint32_t)s[2] * a + (uint32_t)d[2] * inv_a;

        uint8_t b0 = (uint8_t)((v0 + 127u) / 255u);
        uint8_t b1 = (uint8_t)((v1 + 127u) / 255u);
        uint8_t b2 = (uint8_t)((v2 + 127u) / 255u);
        uint8_t b3 = 255u;

        hash ^= b0; hash *= prime;
        hash ^= b1; hash *= prime;
        hash ^= b2; hash *= prime;
        hash ^= b3; hash *= prime;

        s += 4;
        d += 4;
    }
    return hash;
}
