#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
    size_t pixels = (size_t)width * (size_t)height;
    uint64_t hash = 0;
    const uint64_t fnv_prime = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        const uint8_t* s = src;
        const uint8_t* d = dst;
        for (size_t i = 0; i < pixels; ++i) {
            uint32_t a = s[3];
            uint32_t inv_a = 255u - a;

            uint32_t r = (uint32_t)s[0] * a + (uint32_t)d[0] * inv_a;
            uint32_t g = (uint32_t)s[1] * a + (uint32_t)d[1] * inv_a;
            uint32_t b = (uint32_t)s[2] * a + (uint32_t)d[2] * inv_a;

            r = (r * 257u + 32768u) >> 16;
            g = (g * 257u + 32768u) >> 16;
            b = (b * 257u + 32768u) >> 16;

            hash ^= (uint64_t)r; hash *= fnv_prime;
            hash ^= (uint64_t)g; hash *= fnv_prime;
            hash ^= (uint64_t)b; hash *= fnv_prime;
            hash ^= 255ULL;      hash *= fnv_prime;

            s += 4;
            d += 4;
        }
    }
    return hash;
}
