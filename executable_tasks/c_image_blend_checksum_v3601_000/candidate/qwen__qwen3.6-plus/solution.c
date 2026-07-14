#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    (void)iters;
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    const uint8_t* s = src;
    const uint8_t* d = dst;

    for (size_t i = 0; i < pixels; ++i) {
        uint8_t a = s[3];
        uint8_t ia = 255u - a;

        uint8_t r = (uint8_t)(((uint32_t)s[0] * a + (uint32_t)d[0] * ia + 127u) / 255u);
        uint8_t g = (uint8_t)(((uint32_t)s[1] * a + (uint32_t)d[1] * ia + 127u) / 255u);
        uint8_t b = (uint8_t)(((uint32_t)s[2] * a + (uint32_t)d[2] * ia + 127u) / 255u);

        hash ^= r; hash *= FNV_PRIME;
        hash ^= g; hash *= FNV_PRIME;
        hash ^= b; hash *= FNV_PRIME;
        hash ^= 255u; hash *= FNV_PRIME;

        s += 4;
        d += 4;
    }
    return hash;
}